/*
 * CGWIC_CommandPU.cpp
 *
 *  Created on: Jan 29, 2013
 *
 *  Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWIC_CommandPU.h"
#include "CGWIC_World.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

struct CGWIC_CommandPU::GCMDPU_hData {
	CGWIC_World* world;
	irrstrwvec outFIFO;
};

CGWIC_CommandPU::CGWIC_CommandPU()
{
	hData = new GCMDPU_hData();
}

CGWIC_CommandPU::~CGWIC_CommandPU()
{
	delete hData;
}

void CGWIC_CommandPU::SetWorld(void * wrld_ptr)
{
	hData->world = reinterpret_cast<CGWIC_World*> (wrld_ptr);
}

irr::core::stringw CGWIC_CommandPU::GetNextOutput()
{
	stringw out = L"";
	if (hData->outFIFO.empty()) return out;
	out = hData->outFIFO[0];
	hData->outFIFO.erase(hData->outFIFO.begin());
	return out;
}

void CGWIC_CommandPU::Store(irr::core::stringw rec, irr::core::stringw str)
{
	rec += ">";
	rec += str;
	hData->outFIFO.push_back(rec);
}

void CGWIC_CommandPU::Process(irr::core::stringw cmd)
{
	std::cout << "[Command Processor] cmd: " << stringc(cmd).c_str() << std::endl;
	CIrrStrParser parse(cmd);
	stringw receiver;
	if (parse.IsDelimPresent(L">"))
		receiver = parse.NextLex(L">",true);
	else receiver = L"DBGLOG";
	stringw icmd = parse.NextLex(L" ",false);
	if (icmd == L"quit") {
		hData->world->quit_msg = true;
	} else if (icmd == L"test") {
		Store(receiver,L"Test Response String");
		irrstrwvec list = parse.ParseToList(L" ");
		for (u32 i=1; i<list.size(); i++) {
			stringw bvs = L"Argument: ";
			bvs += list[i];
			Store(receiver,bvs);
		}
	} else if (icmd == L"randomplace") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 3) {
			Store(receiver,L"Use: randomplace filename count");
			return;
		}
		CGWIC_Cell* ptr = hData->world->GetCell(hData->world->center_cell);
		CIrrStrParser pr2(list[2]);
		ptr->RandomPlaceObjects(pr2.ToS32(),list[1]);
	} else if (icmd == L"tpm") {
		Store(receiver,L"toggle physics mode");
		hData->world->physicsPause ^= true;
	} else if (icmd == L"tdd") {
		Store(receiver,L"toggle debug drawing");
		hData->world->debugDraw ^= true;
	} else if (icmd == L"reloadcell") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 3) {
			Store(receiver,L"Insufficient args. Use: reloadcell X Y");
			return;
		}
		CIrrStrParser pr2(list[1]);
		s32 clx = pr2.ToS32(); pr2 = list[2];
		s32 cly = pr2.ToS32();
		CGWIC_Cell* clptr = hData->world->GetCell(clx,cly);
		if (!clptr) Store(receiver,L"Cell not found!");
		else hData->world->ReloadCell(clptr);
	} else if (icmd == L"stitch") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 5) {
			Store(receiver,L"Insufficient args. Use: stitch Xa Ya Xb Yb");
			return;
		}
		CIrrStrParser pr2(list[1]);
		CPoint2D aa,bb;
		aa.X = pr2.ToS32();
		pr2 = list[2]; aa.Y = pr2.ToS32();
		pr2 = list[3]; bb.X = pr2.ToS32();
		pr2 = list[4]; bb.Y = pr2.ToS32();
		hData->world->StitchTerrains(hData->world->GetCell(aa),
				hData->world->GetCell(bb),true);
		Store(receiver,L"Command done!");
	} else if (icmd == L"restitch") {
		hData->world->StitchWorld(false);
		Store(receiver,L"Command done!");
	} else if (icmd == L"createwindow") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 2) {
			Store(receiver,L"Use: createwindow <filename.xml>");
			return;
		}
		if (hData->world->CreateNewWindow(list[1]))
			Store(receiver,L"Command done!");
	} else if (icmd == L"randomterr") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 2) {
			Store(receiver,L"Use: randomterr cellX cellY subdelta");
			Store(receiver,L"Or worldwide: randomterr subdelta");
			return;
		}
		CIrrStrParser pr2(list[1]);
		float subd;
		if (list.size() > 3) {
			s32 clx = pr2.ToS32(); pr2 = list[2];
			s32 cly = pr2.ToS32(); pr2 = list[3];
			subd = pr2.ToFloat();
			CGWIC_Cell* cptr = hData->world->GetCell(clx,cly);
			if (cptr) {
				cptr->RandomizeTerrain(subd);
				Store(receiver,L"Cell randomized");
			}
		} else {
			subd = pr2.ToFloat();
			for (u32 xy=0; xy<hData->world->cells.size(); xy++)
				hData->world->cells[xy]->RandomizeTerrain(subd);
			Store(receiver,L"World randomized");
		}
		hData->world->StitchWorld(true);
		hData->world->SunFlick();
	} else if (icmd == L"attachplayername") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() != 2) {
			Store(receiver,L"Use: attachplayername <actor's name to attach to>");
			return;
		}
		for (u32 i=0; i<hData->world->actors.size(); i++)
			if (hData->world->actors[i]->GetName().equals_ignore_case(list[1])) {
				Store(receiver,L"Attaching to");
				Store(receiver,list[1]);
				if (hData->world->PC) {
					hData->world->PC->SetMaster(hData->world->actors[i]);
					hData->world->PC->QuantumUpdate();
				}
			}
	} else if (icmd == L"annulateterrachanges") {
		for (u32 i=0; i<hData->world->cells.size(); i++)
			hData->world->cells[i]->RemoveChangedFlag();
		Store(receiver,L"Terrain change flags removed! Terrain will not be saved.");
	} else if (icmd == L"setdistantland") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() != 2) {
			Store(receiver,L"Use: setdistantland [true/false]");
			return;
		}
		if (list[1] == L"true") {
			Store(receiver,L"Distant land ON");
			hData->world->properties.hardcull.DistantLand = true;
		} else {
			Store(receiver,L"Distant land OFF");
			hData->world->properties.hardcull.DistantLand = false;
		}
	} else if (icmd == L"flushcell") {
		CGWIC_Cell* ccl = hData->world->GetCell(hData->world->center_cell);
		if (ccl) ccl->DeleteObjects();
	} else if (icmd == L"getobjcount") {
		CGWIC_Cell* ccl = hData->world->GetCell(hData->world->center_cell);
		if (ccl) Store(receiver,stringw(ccl->GetObjectsCount()));
	} else {
		Store(receiver,L"Invalid command!");
	}
}

} /* namespace gwic */
