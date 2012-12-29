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

CGWIC_Cell::CGWIC_Cell(CPoint2D pos, irr::IrrlichtDevice* dev, irrBulletWorld* phy)
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
	//TODO: next inits must be get thru parametric init
	maxLOD = 5;
	maxPATCH = ETPS_17;
	terraSmooth = 4;
	waterLevel = -10.f * GWIC_IRRUNITS_PER_METER;
	maxHeight = 4.f; // height scale factor
	heaven = 4000.f * GWIC_IRRUNITS_PER_METER; // upper limit 4km
	hell = -heaven; // lower limit
}

CGWIC_Cell::~CGWIC_Cell()
{
	std::cout << "Destroying cell [" << posX;
	std::cout << ";" << posY << "] ..." << std::endl;
	if (terra_changed) {
		path flnm = "cell";
		flnm += posX;
		flnm += '-';
		flnm += posY;
		flnm += ".bmp";
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
	if (terra_collision) delete terra_collision;
	SetActive(false);
	if (terrain) terrain->remove();
	//TODO: save objects to XML
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
		DeleteObjects();
	}
	active = on;
}

bool CGWIC_Cell::GetActive()
{
	return this->active;
}

bool CGWIC_Cell::InitLand()
{
	std::cout << "InitLand() called" << std::endl;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	vector3df pos(posX*dim,waterLevel,posY*dim);
	vector3df csize(GWIC_IRRUNITS_PER_METER,maxHeight,GWIC_IRRUNITS_PER_METER);
	SColor vcolor(255,255,255,255);
	path flnm = "cell";
	flnm += posX;
	flnm += '-';
	flnm += posY;
	flnm += ".bmp";
	terrain = scManager->addTerrainSceneNode(flnm,NULL,GWIC_PICKABLE_MASK,pos,vector3df(0),csize,vcolor,maxLOD,maxPATCH,terraSmooth);
	if (!terrain) {
		std::cerr << "Terrain for cell " << posX << ';' << pos.Y << " not found!";
		std::cerr << "Using default terrain mesh." << std::endl;
		flnm = "default_terrain.bmp";
		terrain = scManager->addTerrainSceneNode(flnm,NULL,GWIC_PICKABLE_MASK,pos,vector3df(0),csize,vcolor,maxLOD,maxPATCH,terraSmooth);
		if (!terrain) {
			std::cerr << "Unable to create terrain for cell " << posX << ';' << posY << std::endl;
			return false;
		}
	}
	terrain->setMaterialFlag(EMF_LIGHTING,true);
	terrain->setMaterialType(EMT_SOLID);
	terrain->setMaterialTexture(0,irDriver->getTexture("synthgrass.jpg"));
	terrain->scaleTexture(16.f,16.f);
	return true;
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
		objects.push_back(nobj);
		nobj->SetPos(rndpos);
		nobj->SetEnabled(true);
		nobj->SetPhysical(true);
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
	//TODO: fix the objects falled through terrain
	//TODO: process object's CPUs
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
	for (u32 n=0; n<pMesh->getMeshBufferCount(); n++) {
		IMeshBuffer* pMeshBuffer = pMesh->getMeshBuffer(n);
		if (pMeshBuffer->getVertexType() != EVT_2TCOORDS) continue;
		S3DVertex2TCoords* pVertices = (S3DVertex2TCoords*)pMeshBuffer->getVertices();
		u32 idx,x,z;
		float nx,delta,nz;
		float ox = pVertices[0].Pos.Y;
		for (x=0; x<256; x++) {
			delta = subdelta * 3;
			nx = ox + Random_FLOAT(delta);
			nx -= Random_FLOAT(delta);
			delta = 0;
			nz = nx;
			for (z=0; z<256; z++) {
				delta += Random_FLOAT(subdelta);
				delta -= Random_FLOAT(subdelta);
				nz += delta;
				idx = x * 256 + z;
				pVertices[idx].Pos.Y = nz;
			}
		}
		break;
	}
	TerrainChanged();
}


}
