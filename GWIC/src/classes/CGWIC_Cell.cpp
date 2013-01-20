/*
 * CGWIC_Cell.cpp
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWIC_Cell.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_Cell::CGWIC_Cell(CPoint2D pos, const GWICCellParameters params, irr::IrrlichtDevice* dev, irrBulletWorld* phy)
{
	std::cout << "Creating cell [" << pos.X;
	std::cout << ";" << pos.Y << "] ..." << std::endl;
	graphics = dev;
	physics = phy;
	scManager = dev->getSceneManager();
	irDriver = dev->getVideoDriver();
	phy_world = phy;
	posX = pos.X;
	posY = pos.Y;
	active = false;
	terrain = NULL;
	terra_collision = NULL;
	terra_changed = false;
	visible = true;
	maxLOD = params.LOD;
	maxPATCH = params.patch;
	terraSmooth = params.smooth;
	maxHeight = params.heightscale;
	heaven = params.upperlim * GWIC_IRRUNITS_PER_METER;
	waterLevel = params.waterLevel * GWIC_IRRUNITS_PER_METER;
	hell = waterLevel - 10.f * GWIC_IRRUNITS_PER_METER; // lower limit
	groundLevel = params.basepoint * GWIC_IRRUNITS_PER_METER;
	groundTex = NULL;
	initParams = params;
}

CGWIC_Cell::~CGWIC_Cell()
{
	std::cout << "Destroying cell [" << posX;
	std::cout << ";" << posY << "] ..." << std::endl;
	if (terra_collision) delete terra_collision;
	SetActive(false);
	if (terrain) {
		if (terra_changed) SaveTerrainBitmap();
		terrain->remove();
	}
	SaveObjectStates();
	DeleteObjects();
}

CPoint2D CGWIC_Cell::GetCoord()
{
	return CPoint2D(this->posX,this->posY);
}

void CGWIC_Cell::SetActive(bool on)
{
	if (on == active) return;
	if (!terrain) return;
	std::cout << "Setting activation for cell " << posX;
	std::cout << " : " << posY;
	std::cout << " -> " << on << std::endl;
	if (on) {
		vector3df ps = getIrrlichtCenter();
		ps.Y = 100 * GWIC_IRRUNITS_PER_METER;
		ournodes.push_back(scManager->addLightSceneNode(NULL,ps,SColorf(1.f,1.f,1.f),1600,1));
		IMesh* phyterr = TerrainToMesh(2);
		ournodes.push_back(scManager->addMeshSceneNode(phyterr));
		ournodes.back()->setPosition(terrain->getPosition());
		ournodes.back()->setVisible(false);
		terra_collision = new IBvhTriangleMeshShape(ournodes.back(),phyterr,0.f);
		phyterr->drop();
		if (terra_collision)
			ourphys.push_back(phy_world->addRigidBody(terra_collision));
		LoadObjectStates();
	} else {
		while (ourphys.size() > 0) {
			phy_world->removeCollisionObject(ourphys.back(),true);
			//ourphys.back()->remove();
			ourphys.pop_back();
		}
		while (ournodes.size() > 0) {
			ournodes.back()->remove();
			ournodes.pop_back();
		}
		terrain->setTriangleSelector(NULL);
		SaveObjectStates();
		DeleteObjects();
	}
	active = on;
}

bool CGWIC_Cell::InitLand()
{
	std::cout << "InitLand() called" << std::endl;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	vector3df pos(posX*dim,groundLevel,posY*dim);
	vector3df csize(GWIC_IRRUNITS_PER_METER,maxHeight,GWIC_IRRUNITS_PER_METER);
	SColor vcolor(255,255,255,255);
	path flnm = GWIC_TERRAIN_DIR + "cell";
	flnm += posX;
	flnm += '-';
	flnm += posY;
	flnm += ".bmp";
	terrain = scManager->addTerrainSceneNode(flnm,NULL,GWIC_PICKABLE_MASK,pos,vector3df(0),csize,vcolor,maxLOD,maxPATCH,terraSmooth);
	if (!terrain) {
		std::cerr << "Terrain for cell " << posX << ';' << pos.Y << " not found!";
		std::cerr << "Using default terrain mesh." << std::endl;
		flnm = GWIC_TERRAIN_DIR + "default_terrain.bmp";
		terrain = scManager->addTerrainSceneNode(flnm,NULL,GWIC_PICKABLE_MASK,pos,vector3df(0),csize,vcolor,maxLOD,maxPATCH,terraSmooth);
		visible = false;
		if (!terrain) {
			std::cerr << "Unable to create terrain for cell " << posX << ';' << posY << std::endl;
			return false;
		}
	}
	terrain->setMaterialFlag(EMF_LIGHTING,true);
	terrain->setMaterialType(EMT_SOLID);
	groundTex = irDriver->getTexture(GWIC_TEXTURES_DIR+initParams.txdpath);
	if (!groundTex) return false;
	terrain->setMaterialTexture(0,groundTex);
	terrain->scaleTexture(initParams.texrepeats);
	return true;
}

void CGWIC_Cell::SetVisible(const bool enable)
{
	if (terrain) {
		visible = enable;
		terrain->setVisible(enable);
	}
}

void CGWIC_Cell::DeleteObjects()
{
	while (objects.size() > 0) {
		delete (objects.back());
		objects.pop_back();
	}
	while (transferFIFO.size() > 0) {
		delete (transferFIFO.back());
		transferFIFO.pop_back();
	}
}

void CGWIC_Cell::RandomPlaceObjects(int count, irr::io::path filename)
{
	CGWIC_GameObject* nobj;
	CPoint2D werhere(posX,posY);
	vector3df rndpos;
	//FIXME: check existence of file first!
	for (int i=0; i<count; i++) {
		rndpos.X = Random_FLOAT(GWIC_METERS_PER_CELL);
		rndpos.Z = Random_FLOAT(GWIC_METERS_PER_CELL);
		rndpos.Y = GetTerrainHeightUnderPointMetric(rndpos) + 1.f;
		nobj = new CGWIC_GameObject(filename,werhere,graphics,physics);
		if (nobj->GetRootNode()) {
			objects.push_back(nobj);
			nobj->SetPos(rndpos);
			nobj->SetEnabled(true);
			nobj->SetPhysical(true);
		} else
			std::cerr << "Couldn't create new object!" << std::endl;
	}
}

irr::core::vector3df CGWIC_Cell::getIrrlichtCenter()
{
	float cent = GWIC_METERS_PER_CELL / 2;
	vector3df res(cent,0,cent);
	res *= GWIC_IRRUNITS_PER_METER;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	res.X += (dim * posX);
	res.Z += (dim * posY);
	res.Y = terrain->getAbsolutePosition().Y + 128.f;
	return res;
}

irr::scene::IMesh* CGWIC_Cell::TerrainToMesh(int LOD)
{
	SMesh* out = new SMesh();
	CDynamicMeshBuffer* buff = new CDynamicMeshBuffer(EVT_2TCOORDS,EIT_16BIT);
	terrain->getMeshBufferForLOD(*buff,LOD);
	const u32 vertCnt = buff->getVertexCount();
	S3DVertex2TCoords* mbv = reinterpret_cast<S3DVertex2TCoords*> (buff->getVertexBuffer().getData());
	vector3df scale = terrain->getScale();
	for (u32 i=0; i<vertCnt; i++) mbv[i].Pos *= scale;
	out->addMeshBuffer(buff);
	out->recalculateBoundingBox();
	buff->drop();
	terrain->setPosition(terrain->getPosition());
	return out;
}

void CGWIC_Cell::Update()
{
	vector3df cpos;
	CPoint2D ccl;
	CGWIC_GameObject* ptr;
	float dim = GWIC_METERS_PER_CELL;// * GWIC_IRRUNITS_PER_METER;
	for (u32 i=0; i<objects.size(); i++) {
		ptr = NULL;
		objects[i]->QuantumUpdate();
		cpos = objects[i]->GetPos();
		ccl = objects[i]->GetCell();
		if (cpos.Y < hell) {
			std::cout << "going to hell ;)" << std::endl;
			RemoveObjectByNum(i--);
			continue;
		} else if (cpos.Y > heaven) {
			std::cout << "you're in heaven" << std::endl;
			RemoveObjectByNum(i--);
			continue;
		}
		if ((objects[i]->GetPhysical()) &&
			(cpos.Y - GetTerrainHeightUnderPointMetric(cpos) < -GWIC_METERS_PER_CELL)) {
				cpos.Y = GetTerrainHeightUnderPointMetric(cpos);
				objects[i]->SetPos(cpos);
			}
		if (cpos.X < 0) {
			std::cout << "transfer left" << std::endl;
			ccl.X--;
			cpos.X += dim;
			ptr = objects[i];
		} else if (cpos.Z < 0) {
			std::cout << "transfer up" << std::endl;
			ccl.Y--;
			cpos.Z += dim;
			ptr = objects[i];
		}
		if (cpos.X > GWIC_METERS_PER_CELL) {
			std::cout << "transfer right" << std::endl;
			ccl.X++;
			cpos.X -= dim;
			ptr = objects[i];
		} else if (cpos.Z > GWIC_METERS_PER_CELL) {
			std::cout << "transfer down" << std::endl;
			ccl.Y++;
			cpos.Z -= dim;
			ptr = objects[i];
		}
		objects[i]->SetCell(ccl);
		if (ptr) {
			transferFIFO.insert(transferFIFO.begin(),ptr);
			objects.erase(objects.begin()+i);
		}
	}
}

CGWIC_GameObject* CGWIC_Cell::PopTransferObject()
{
	if (transferFIFO.size() < 1) return NULL;
	CGWIC_GameObject* optr = transferFIFO.back();
	transferFIFO.pop_back();
	return optr;
}

void CGWIC_Cell::PushTransferObject(CGWIC_GameObject* optr)
{
	if (!optr) return;
	objects.push_back(optr);
	optr->SetEnabled(active);
}

bool CGWIC_Cell::RemoveObjectByNum(irr::u32 num)
{
	if (num >= objects.size()) return false;
	CGWIC_GameObject* ptr = objects[num];
	objects.erase(objects.begin()+num);
	if (!ptr) return false;
	delete ptr;
	return true;
}

bool CGWIC_Cell::RemoveObjectByPtr(CGWIC_GameObject* ptr)
{
	for (u32 i=0; i<objects.size(); i++)
		if (objects[i] == ptr)
			return (RemoveObjectByNum(i));
	return false;
}

float CGWIC_Cell::GetTerrainHeightUnderPointMetric(irr::core::vector3df pnt)
{
	u32 x = floor(pnt.X);
	u32 z = floor(pnt.Z);
	if (x > 255) x = 255;
	if (z > 255) z = 255;
	u32 index = x * 256 + z;
	IMesh* pMesh = terrain->getMesh();
	float res = 0.f;
//	float max = 0;
	const float scy = terrain->getScale().Y;
	//FIXME: get the true position in meters
	for (u32 n=0; n<pMesh->getMeshBufferCount(); n++) {
		IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(n);
		if (pMeshBuffer->getVertexType() != EVT_2TCOORDS) continue;
		S3DVertex2TCoords* pVertices = (S3DVertex2TCoords*)pMeshBuffer->getVertices();
		res = pVertices[index].Pos.Y / scy;
//		res = (pVertices[index].Pos.Y + terrain->getPosition().Y) / scy;
//		res += terrain->getPosition().Y / GWIC_IRRUNITS_PER_METER;
//		std::cerr << "POSIT  " << res << std::endl;
//		for (u32 k=0; k<(256*256); k++) if (pVertices[k].Pos.Y > max) max = pVertices[k].Pos.Y;
//		std::cerr << "MAXX  " << max << std::endl;
		break;
	}
	return res;
}

bool CGWIC_Cell::SetTerrainHeightUnderPointMetric(irr::core::vector3df pnt, float height, bool update)
{
	u32 x = floor(pnt.X);
	u32 z = floor(pnt.Z);
	if (x > 255) x = 255;
	if (z > 255) z = 255;
	u32 index = x * 256 + z;
	IMesh* pMesh = terrain->getMesh();
	const float scy = terrain->getScale().Y;
	for (u32 n=0; n<pMesh->getMeshBufferCount(); n++) {
		IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(n);
		if (pMeshBuffer->getVertexType() != EVT_2TCOORDS) continue;
		S3DVertex2TCoords* pVertices = (S3DVertex2TCoords*)pMeshBuffer->getVertices();
		pVertices[index].Pos.Y = height * scy;
		break;
	}
	if (update) TerrainChanged();
	return true;
}

CGWIC_GameObject* CGWIC_Cell::GetObjectByIrrPtr(irr::scene::ISceneNode* ptr)
{
	for (u32 i=0; i<objects.size(); i++)
		if (objects[i]->GetRootNode() == ptr) return (objects[i]);
	return NULL;
}

CGWIC_GameObject* CGWIC_Cell::GetObjectByNum(irr::u32 num)
{
	if (num >= objects.size()) return NULL;
	return (objects[num]);
}

void CGWIC_Cell::TerrainChanged()
{
	terrain->setPosition(terrain->getPosition());
	//I've decided to not regenerate physics every time vertices moving. It's too hard for CPU :)
	//uncomment to save the map
	terra_changed = true;
}

void CGWIC_Cell::RandomizeTerrain(float subdelta)
{
	IMesh* pMesh = terrain->getMesh();
	s32 Q = static_cast<s32> (subdelta);
	for (u32 n=0; n<pMesh->getMeshBufferCount(); n++) {
		IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(n);
		if (pMeshBuffer->getVertexType() != EVT_2TCOORDS) continue;
		S3DVertex2TCoords* pVertices = (S3DVertex2TCoords*)pMeshBuffer->getVertices();
		//1. find the current max & min, since we don't want to escalate landscape
		float max = 0;
		float min = 256;
		u32 i;
		for (i=0; i<(256*256); i++) {
			if (pVertices[i].Pos.Y < min) min = pVertices[i].Pos.Y;
			if (pVertices[i].Pos.Y > max) max = pVertices[i].Pos.Y;
		}
		std::cout << "RandTerr: min=" << min << ";  max=" << max << std::endl;
		//2. create temp array & randomize it
		std::vector<float> tmparr;
		float cy;
		for (i=0; i<(256*256); i++) {
			if (Q) {
				cy = Random_FLOAT(max-min);
				cy += min;
			} else cy = 0;
			tmparr.push_back(cy);
		}
		std::cout << std::endl;
		//3. interpolate vertices inside big quads
		s32 A = 0;
		if (Q) A = 256 / Q + 1;
		for (int qx=0; qx<A; qx++)
			for (int qy=0; qy<A; qy++) {
				//TODO: remove unnecessary vars
				int x0 = qx * Q;
				int y0 = qy * Q;
				int xe = x0 + Q;
				if (xe > 255) xe = 255;
				int ye = y0 + Q;
				if (ye > 255) ye = 255;
				float hx0 = tmparr[x0*256+y0];
				float hy0 = (qx%2)? tmparr[xe*256+ye]:hx0;
				float hxe = tmparr[xe*256+y0];
				float hye = tmparr[x0*256+ye];
				float lhy = hye - hy0;
				float lhx = hxe - hx0;
				for (int cx=x0; cx<xe; cx++)
					for (int cy=y0; cy<ye; cy++) {
						float ch = (((qx%2)?(x0-cx):(cx-x0))/lhx)*lhx+hx0;
						ch += (((qy%2)?(y0-cy):(cy-y0))/lhy)*lhy+hy0;
						tmparr[cx*256+cy] = ch / 2;
					}
			}
		//4. get result back
		for (i=0; i<(256*256); i++)
			pVertices[i].Pos.Y = tmparr[i];
		break;
	}
	TerrainChanged();
}

void CGWIC_Cell::SaveTerrainBitmap()
{
	path flnm = GWIC_TERRAIN_DIR;
	flnm += (GetCellFileSuffix() + ".bmp");
	dimension2d<u32> dim(256,256);
	IImage *img = irDriver->createImage(ECF_R8G8B8,dim);
	u32 VertexCount = terrain->getMesh()->getMeshBuffer(0)->getVertexCount();
	S3DVertex2TCoords* verts = reinterpret_cast<S3DVertex2TCoords*> (terrain->getMesh()->getMeshBuffer(0)->getVertices());
	S3DVertex2TCoords* vertex;
	u8 y;
	u32 x,z;
	for (u32 i=0; i<VertexCount; i++) {
		vertex = verts + i;
		x = static_cast<u32> (vertex->Pos.X);
		y = static_cast<u8> (vertex->Pos.Y);
		z = static_cast<u32> (vertex->Pos.Z);
		img->setPixel(x,z,SColor(0,y,y,y));
	}
	irDriver->writeImageToFile(img,flnm,0);
	img->drop();
	std::cout << "Cell " << posX << ';' << posY << " terrain bitmap saved!" << std::endl;
}

void CGWIC_Cell::SaveObjectStates()
{
	path filenm; //FIXME: add cells storage path
	filenm += GetCellFileSuffix();
	filenm += ".xml";
	IXMLWriter* xml = graphics->getFileSystem()->createXMLWriter(filenm);
	if (!xml) {
		std::cerr << "SaveObjectStates(): can't create writer to " << filenm.c_str() << std::endl;
		return;
	}
	xml->drop();
}

bool CGWIC_Cell::LoadObjectStates()
{
	path filenm; //FIXME: add cells storage path
	filenm += GetCellFileSuffix();
	filenm += ".xml";
	IXMLReader* xml = graphics->getFileSystem()->createXMLReader(filenm);
	if (!xml) {
		std::cerr << "LoadObjectStates(): can't create xml reader for " << filenm.c_str() << std::endl;
		return false;
	}
	xml->drop();
	return false;
}

irr::core::stringw CGWIC_Cell::GetCellFileSuffix()
{
	stringw out = "cell";
	out += posX;
	out += '-';
	out += posY;
	return out;
}


}
