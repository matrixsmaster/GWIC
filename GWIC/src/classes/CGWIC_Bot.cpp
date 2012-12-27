/*
 * CGWIC_Bot.cpp
 *  Created on: Dec 3, 2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWIC_Bot.h"
#include "CGWIC_Cell.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

namespace gwic {

CGWIC_Bot::CGWIC_Bot(BotCreationParams* params, irr::IrrlichtDevice* dev, irrBulletWorld* phy)
{
	LogIt("Bot create()");
	irDevice = dev;
	scManager = dev->getSceneManager();
	irDriver = dev->getVideoDriver();
	phy_world = phy;
	// init actor
	initDone = false;
	enabled = false;
	basicShell = NULL;
	botRoot = NULL;
	botShell = NULL;
	head = NULL;
	initParams = *params;
	mycell = params->cell_coord;
	myname = params->actorname;
	IAnimatedMesh* botmesh = NULL;
	IAnimatedMeshSceneNode* animnode = NULL;
	switch (params->type) {
	case ACTOR_DUMMY:
		// get Sydney to the scene ;)
		botmesh = scManager->getMesh("sydney.md2");
		if (!botmesh) return;
		animnode = scManager->addAnimatedMeshSceneNode(botmesh);
		botRoot = animnode;
		botRoot->setMaterialType(EMT_REFLECTION_2_LAYER);
		botRoot->setMaterialTexture(0,irDriver->getTexture("sydney.bmp"));
		botRoot->setMaterialTexture(1,irDriver->getTexture("spheremap.jpg"));
		basicShell = new IBoxShape(botRoot,60.f,false);
		break;
	case ACTOR_GYNOID:
		head = CreateNPC(params->filename);
		break;
	case ACTOR_PLAYER:
		//TODO: create capsule and load basic model
		return;
	case ACTOR_CREATURE:
		//TODO: load classic skinned rigged mesh
		return;
	default:
		std::cerr << "Unsupported or invalid actor type creation requested!" << std::endl;
		return;
	}
	if (animnode) {
		ITriangleSelector* sel = scManager->createTriangleSelector(animnode);
		animnode->setTriangleSelector(sel);
		sel->drop();
	}
	mHeight = 1.f;
	initDone = true;
	AutoSize();
	SetPos(params->rel_pos);
	if (head) head->RebuildPhysics(true);
}

CGWIC_Bot::~CGWIC_Bot()
{
	LogIt(GetTypeAsString()+" bot destroy()");
	if (enabled) SetEnabled(false);
	if (head) delete head;
	if (botRoot) botRoot->remove();
	if (basicShell) delete basicShell;
}

void CGWIC_Bot::LogIt(irr::core::stringc msg)
{
	std::cout << "[GWIC_Bot]: " << msg.c_str() << std::endl;
}

bool CGWIC_Bot::SetPos(irr::core::vector3df rel_pos)
{
	if (!initDone) return false;
	position = rel_pos; //remember it in meters
	//position.Y += mHeight / 2;
	if (botRoot) botRoot->setPosition(getAbsPosition());
	if (botShell) {
		btTransform btt = botShell->getPointer()->getCenterOfMassTransform();
		btt.setOrigin(irrlichtToBulletVector(getAbsPosition()));
		botShell->getPointer()->setCenterOfMassTransform(btt);
		if (botShell->getActivationState() == EAS_SLEEPING)
			botShell->setActivationState(EAS_ACTIVE);
	}
	if (head) head->Move(getAbsPosition());
	return true;
}

irr::core::vector3df CGWIC_Bot::GetPos()
{
	vector3df mpos(position);
	//mpos.Y -= mHeight / 2;
	return mpos;
}

bool CGWIC_Bot::SetCell(CPoint2D ncl)
{
	//
	return true;
}

irr::core::vector3df CGWIC_Bot::getAbsPosition()
{
	vector3df res(position);
	res *= GWIC_IRRUNITS_PER_METER;
	float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	res.X += (dim * mycell.X);
	res.Y += mHeight / 2;
	res.Z += (dim * mycell.Y);
	return res;
}

bool CGWIC_Bot::SetRot(irr::core::vector3df rot)
{
	//
	return true;
}

irr::core::vector3df CGWIC_Bot::GetRot()
{
	vector3df res(0,0,0);
	return res;
}

bool CGWIC_Bot::SetMaterial(ObjMaterial newmat)
{
	//
	return true;
}

ObjMaterial CGWIC_Bot::GetMaterial()
{
	return this->botmat;
}

void CGWIC_Bot::SetEnabled(bool enable)
{
	if (enable == enabled) return;
	if (enable) {
		if (basicShell) botShell = phy_world->addRigidBody(basicShell);
	} else {
		if (botShell) phy_world->removeCollisionObject(botShell,true);
	}
	if (botRoot) botRoot->setVisible(enable);
	enabled = enable;
}

bool CGWIC_Bot::GetEnabled()
{
	//
	return true;
}

void CGWIC_Bot::AutoSize()
{
	if (!initDone) return;
	float eY = 0;
	if (botRoot) eY = botRoot->getBoundingBox().getExtent().Y;
	if (head) {
		float eey = head->GetMaxExtent().Y;
		if (eey > eY) eY = eey;
	}
	float k = initParams.height * GWIC_IRRUNITS_PER_METER / eY;
	mHeight = eY * k;
	stringc vs = "AutoSize: k:  ";
	vs += k;
	LogIt(vs);
	vector3df nscal(k);
	if (botRoot) nscal = botRoot->getScale() * k;
	if (head) head->SetScale(nscal,true);
	else if (botRoot) botRoot->setScale(nscal);
}

irr::core::stringc CGWIC_Bot::GetTypeAsString()
{
	switch (initParams.type) {
	case ACTOR_DUMMY: return "Dummy";
	case ACTOR_GYNOID: return "Gynoid";
	case ACTOR_PLAYER: return "Player";
	case ACTOR_CREATURE: return "Creature";
	default: return "Unknown";
	}
}

CGWIC_Head* CGWIC_Bot::CreateNPC(irr::core::stringw file)
{
	CGWIC_BodyPart* cpart = NULL;
	CGWIC_Head* nhead = NULL;
	/* There was strange error: if we create root node and then move it, geometry moves too,
	 * but boundbox and absolute position remains the same, even with updateAbsolutePosition()
	 * calls, so I've decided to not to handle entire bot structure as Irrlicht's hierarchy
	 * and use our internal GWIC classes facilities to handle the tree, move it, scale, hide, etc
	 */
//	botRoot = scManager->addEmptySceneNode(NULL,0);
	nhead = new CGWIC_Head("a_head.xml",botRoot,irDevice,phy_world);
	cpart = new CGWIC_BodyPart("a_torso.xml",botRoot,irDevice,phy_world);
	cpart->SetName(L"torso");
	nhead->Connect(cpart,1);
	cpart->Connect(nhead,0); //parent part back ref
	cpart = new CGWIC_BodyPart("a_larm.xml",botRoot,irDevice,phy_world);
	cpart->SetName(L"larm");
	CGWIC_BodyPart* cprt2 = nhead->GetNodeByName(L"torso");
	cprt2->Connect(cpart,1);
	cpart->Connect(cprt2,0); //back ref
	nhead->SetActive(true,true);
	return nhead;
}

irr::s32 CGWIC_Bot::IsThisNodeIsMine(irr::scene::ISceneNode* node)
{
	if (botRoot == node) return 1;
	if (head) return (head->RecursiveSearchForNode(node));
	return 0;
}


}
