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
#include "CGWIC_Cell.h"
#include "CGWIC_GameObject.h"
#include "CGWIC_Bot.h"

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

void CGWIC_CommandPU::Error(irr::core::stringw rec, irr::core::stringw str)
{
	stringw out(L"DBGLOG>");
	out += L"to{";
	out += rec;
	out += L"}: ";
	out += str;
	hData->outFIFO.push_back(out);
}

void CGWIC_CommandPU::Process(irr::core::stringw cmd)
{
	std::cout << "[Command Processor] cmd: " << stringc(cmd).c_str() << std::endl;
	GWIC_CMDPU_cmdata icmd;
	CIrrStrParser parse(cmd);
	icmd.parse = parse;
	if (parse.IsDelimPresent(L">"))
		icmd.receiver = parse.NextLex(L">",true);
	else icmd.receiver = L"DBGLOG";
	stringw cmd_str = parse.NextLex(L" ",false);
	GWIC_CMDPU_CMD rcmd;
	s32 i = -1;
	do rcmd = commandPU_commandlist[++i].cmdtype;
	while ((rcmd != GCMD_UNKNOWN) &&
			(cmd_str != stringw(commandPU_commandlist[i].cmdstring.c_str())));
	switch (rcmd) {
	case GCMD_quit:
		hData->world->quit_msg = true;
		break;
	case GCMD_test:
		cmd_test(&icmd);
		break;
	case GCMD_help:
		Store(icmd.receiver,L"there's no help.... yet");
		break;
	case GCMD_randomplace:
		cmd_randomplace(&icmd);
		break;
	case GCMD_tpm:
		Store(icmd.receiver,L"toggle physics mode");
		hData->world->physicsPause ^= true;
		break;
	case GCMD_tdd:
		Store(icmd.receiver,L"toggle debug drawing");
		hData->world->debugDraw ^= true;
		break;
	case GCMD_reloadcell:
		cmd_reloadcell(&icmd);
		break;
	case GCMD_stitch:
		cmd_stitch(&icmd);
		break;
	case GCMD_restitch:
		hData->world->StitchWorld(false);
		Store(L"DBGLOG",L"World re-stitched!");
		break;
	case GCMD_createwindow:
		cmd_createwindow(&icmd);
		break;
	case GCMD_randomterr:
		cmd_randomterr(&icmd);
		break;
	case GCMD_attachplayername:
		cmd_attachplayername(&icmd);
		break;
	case GCMD_annulateterrachanges:
		for (u32 i=0; i<hData->world->cells.size(); i++)
			hData->world->cells[i]->RemoveChangedFlag();
		Store(icmd.receiver,L"Terrain change flags removed! Terrain will not be saved.");
		break;
	case GCMD_setdistantland:
		cmd_setdistantland(&icmd);
		break;
	case GCMD_flushcell:
		cmd_flushcell(&icmd);
		break;
	case GCMD_getobjcount:
		cmd_getobjcount(&icmd);
		break;
	case GCMD_placeatcam:
		cmd_placeatcam(&icmd);
		break;
	case GCMD_listactornames:
		cmd_listactornames(&icmd);
		break;
	case GCMD_lightatcam_test:
		cmd_lightatcam_test(&icmd);
		break;
	case GCMD_sunatcam:
		cmd_sunatcam(&icmd);
		break;
	default:
		Store(icmd.receiver,L"Unknown command!");
		break;
	}
}


void CGWIC_CommandPU::cmd_test(GWIC_CMDPU_cmdata *input)
{
	Store(input->receiver,L"Test Response String");
	irrstrwvec list = input->parse.ParseToList(L" ");
	for (u32 i=1; i<list.size(); i++) {
		stringw bvs = L"Argument: ";
		bvs += list[i];
		Store(input->receiver,bvs);
	}
}

void CGWIC_CommandPU::cmd_randomplace(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 3) {
		Error(input->receiver,L"Use: randomplace filename count");
		return;
	}
	CGWIC_Cell* ptr = hData->world->GetCell(hData->world->center_cell);
	CIrrStrParser pr2(list[2]);
	ptr->RandomPlaceObjects(pr2.ToS32(),list[1]);
}

void CGWIC_CommandPU::cmd_reloadcell(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 3) {
		Error(input->receiver,L"Insufficient args. Use: reloadcell X Y");
		return;
	}
	CIrrStrParser pr2(list[1]);
	s32 clx = pr2.ToS32(); pr2 = list[2];
	s32 cly = pr2.ToS32();
	CGWIC_Cell* clptr = hData->world->GetCell(clx,cly);
	if (!clptr) Store(input->receiver,L"Cell not found!");
	else hData->world->ReloadCell(clptr);
}

void CGWIC_CommandPU::cmd_stitch(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 5) {
		Error(input->receiver,L"Insufficient args. Use: stitch Xa Ya Xb Yb");
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
	Store(L"DBGLOG",L"Stitch done!");
}

void CGWIC_CommandPU::cmd_createwindow(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 2) {
		Error(input->receiver,L"Use: createwindow <filename.xml>");
		return;
	}
	if (hData->world->CreateNewWindow(list[1]))
		Store(L"DBGLOG",L"Command done!");
}

void CGWIC_CommandPU::cmd_randomterr(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 2) {
		Error(input->receiver,L"Use: randomterr cellX cellY subdelta");
		Error(input->receiver,L"Or worldwide: randomterr subdelta");
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
			Store(input->receiver,L"Cell randomized");
		}
	} else {
		subd = pr2.ToFloat();
		for (u32 xy=0; xy<hData->world->cells.size(); xy++)
			hData->world->cells[xy]->RandomizeTerrain(subd);
		Store(input->receiver,L"World randomized");
	}
	hData->world->StitchWorld(true);
	hData->world->SunFlick();
}

void CGWIC_CommandPU::cmd_attachplayername(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() != 2) {
		Error(input->receiver,L"Use: attachplayername <actor's name to attach to>");
		return;
	}
	for (u32 i=0; i<hData->world->actors.size(); i++)
		if (hData->world->actors[i]->GetName().equals_ignore_case(list[1])) {
			Store(L"DBGLOG",L"Attaching to");
			Store(L"DBGLOG",list[1]);
			if (hData->world->PC) {
				hData->world->PC->SetMaster(hData->world->actors[i]);
				hData->world->PC->QuantumUpdate();
			}
		}
}

void CGWIC_CommandPU::cmd_setdistantland(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() != 2) {
		Error(input->receiver,L"Use: setdistantland [true/false]");
		return;
	}
	if (list[1] == L"true") {
		Store(input->receiver,L"Distant land ON");
		hData->world->properties.hardcull.DistantLand = true;
	} else {
		Store(input->receiver,L"Distant land OFF");
		hData->world->properties.hardcull.DistantLand = false;
	}
}

void CGWIC_CommandPU::cmd_flushcell(GWIC_CMDPU_cmdata *input)
{
	CGWIC_Cell* ccl = hData->world->GetCell(hData->world->center_cell);
	if (ccl) ccl->DeleteObjects();
}

void CGWIC_CommandPU::cmd_getobjcount(GWIC_CMDPU_cmdata *input)
{
	CGWIC_Cell* ccl = hData->world->GetCell(hData->world->center_cell);
	if (ccl) Store(input->receiver,stringw(ccl->GetObjectsCount()));
}

void CGWIC_CommandPU::cmd_placeatcam(GWIC_CMDPU_cmdata *input)
{
	irrstrwvec list = input->parse.ParseToList(L" ");
	if (list.size() < 3) {
		Error(input->receiver,L"Use: placeatcam <type> [filename] [actor_name]");
		return;
	}
	vector3df npos(hData->world->main_cam->getPosition());
	npos /= GWIC_IRRUNITS_PER_METER;
	npos.X = fmod(npos.X,GWIC_METERS_PER_CELL);
	npos.Z = fmod(npos.Z,GWIC_METERS_PER_CELL);
	BotCreationParams params;
	params.cell_coord = hData->world->center_cell;
	params.filename = list[2];
	params.height = 1.7f;
	params.rel_pos = npos;
	bool bot = false;
	if (list[1] == L"object") {
		CGWIC_Cell* ccl = hData->world->GetCell(hData->world->center_cell);
		if (ccl->CreateNewObject(npos,list[2]))
			Store(input->receiver,"GameObject created successfully!");
		else
			Error(input->receiver,L"GameObject creation failed!");
	} else if (list[1] == L"dummy_actor") {
		params.actorname = (list.size() > 2)? list[2]:L"Unnamed_Dummy";
		params.type = ACTOR_DUMMY;
		bot = true;
	} else if (list[1] == L"gynoid") {
		params.actorname = (list.size() > 3)? list[3]:L"Unnamed_Gynoid";
		params.type = ACTOR_GYNOID;
		params.filename = list[2];
		bot = true;
	}
	if (!bot) return;
	CGWIC_Bot* nbot = new CGWIC_Bot(&params,hData->world->gra_world,hData->world->phy_world);
	if (nbot->isCompletelyDead()) {
		Error(input->receiver,L"Actor spawning failed!");
		delete nbot;
		return;
	}
	nbot->SetEnabled(true);
	hData->world->actors.push_back(nbot);
}

void CGWIC_CommandPU::cmd_listactornames(GWIC_CMDPU_cmdata *input)
{
	for (u32 ia=0; ia<hData->world->actors.size(); ia++)
		Store(input->receiver,hData->world->actors[ia]->GetName());
}

void CGWIC_CommandPU::cmd_lightatcam_test(GWIC_CMDPU_cmdata *input)
{
	//do not use this function unless you know what you're doing!
	ICameraSceneNode* cam = hData->world->main_cam;
	if (cam)
		hData->world->scManager->addLightSceneNode(NULL,cam->getAbsolutePosition(),SColorf(1.f,1.f,1.f),1600,1);
}

void CGWIC_CommandPU::cmd_sunatcam(GWIC_CMDPU_cmdata *input)
{
	ICameraSceneNode* cam = hData->world->main_cam;
	if (cam)
		hData->world->theSun->setPosition(cam->getAbsolutePosition());
}


} /* namespace gwic */
