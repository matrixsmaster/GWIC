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
	visible = true;
	basicShell = NULL;
	botRoot = NULL;
	botShell = NULL;
	head = NULL;
	initParams = *params;
	mycell = params->cell_coord;
	myname = params->actorname;
	master_bot = NULL;
	headcam = NULL;
	camc_tries = 0;
	mHeight = 1.f;
	IAnimatedMesh* botmesh = NULL;
	IAnimatedMeshSceneNode* animnode = NULL;
	switch (params->type) {
	case ACTOR_DUMMY:
		// get Sydney to the scene ;)
		botmesh = scManager->getMesh("sydney.md2");
		if (!botmesh) return;
		animnode = scManager->addAnimatedMeshSceneNode(botmesh);
		botRoot = animnode;
//		botRoot->setMaterialType(EMT_REFLECTION_2_LAYER);
		botRoot->setMaterialTexture(0,irDriver->getTexture("sydney.bmp"));
//		botRoot->setMaterialTexture(1,irDriver->getTexture("spheremap.jpg"));
//		animnode->addShadowVolumeSceneNode();
		animnode->setMD2Animation(EMAT_STAND);
		basicShell = new IBoxShape(botRoot,60.f,false);
		break;
	case ACTOR_GYNOID:
		head = CreateNPC(params->filename);
		break;
	case ACTOR_PLAYER:
		/*
		 * Player char is not a graphical and/or physical one!
		 * We can attach PC to any other actor on demand
		 */
		mHeight = 0.001f;
		botRoot = scManager->addEmptySceneNode(NULL,0);
		botRoot->setScale(vector3df(1.f/GWIC_IRRUNITS_PER_METER));
		break;
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
	initDone = true;
	if (params->type != ACTOR_PLAYER)
		AutoSize();
	SetPos(params->rel_pos);
	if (head) head->RebuildPhysics(true);
}

CGWIC_Bot::~CGWIC_Bot()
{
	LogIt(GetTypeAsString()+" bot destroy()");
	if (enabled) SetEnabled(false);
	if (headcam) headcam->remove();
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
//	position.Y -= mHeight / 2;
	if (botRoot) botRoot->setPosition(getAbsPosition());
	if (botShell) {
		btTransform btt = botShell->getPointer()->getCenterOfMassTransform();
		btt.setOrigin(irrlichtToBulletVector(getAbsPosition()));
		botShell->getPointer()->setCenterOfMassTransform(btt);
		if (botShell->getActivationState() == EAS_SLEEPING)
			botShell->setActivationState(EAS_ACTIVE);
	}
	if (head) {
		rel_pos = getAbsPosition() - head->GetRealPosition();
		head->Move(rel_pos);
	}
	return true;
}

irr::core::vector3df CGWIC_Bot::GetPos()
{
	bool chng = false;
	if (botRoot) {
		position = botRoot->getPosition();
		chng = true;
	} else if (head) {
		position = head->GetAbsPos();
		chng = true;
	}
	if (chng) {
		position /= GWIC_IRRUNITS_PER_METER;
		mycell.X = floor(position.X / GWIC_METERS_PER_CELL);
		position.X -= mycell.X * GWIC_METERS_PER_CELL;
		mycell.Y = floor(position.Z / GWIC_METERS_PER_CELL);
		position.Z -= mycell.Y * GWIC_METERS_PER_CELL;
	}
	return position;
}

bool CGWIC_Bot::SetCell(CPoint2D ncl)
{
	mycell = ncl;
	return true;
}

//Note: if you need more CURRENT position, call GetPos() instead!
irr::core::vector3df CGWIC_Bot::getAbsPosition()
{
	vector3df res(position); //there's some reason to do it this way
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
		if (headcam) {
			headcam->remove();
			headcam = NULL;
		}
	}
	if (head) head->SetActive(enable,true);
	else if (botRoot) botRoot->setVisible(enable);
	enabled = enable;
}

bool CGWIC_Bot::GetEnabled()
{
	if (head) return head->GetActive();
	else if (botRoot) return botRoot->isVisible();
	else return false;
}

void CGWIC_Bot::SetVisible(const bool enable)
{
	visible = enable;
	if (head) head->SetVisible(enable);
	else if (botRoot) botRoot->setVisible(enable);
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

void CGWIC_Bot::SetMaster(CGWIC_Bot* nwmaster)
{
	if (nwmaster != this) master_bot = nwmaster;
	else master_bot = NULL;
}

/*
 * It's recommended to use this method EVERY time you want to access
 * to bot's head camera.
 * This method creates the camera if bot needs one just on demand :)
 */
irr::scene::ICameraSceneNode* CGWIC_Bot::GetCamera()
{
	if (!headcam) {
		if (camc_tries > 3) return NULL;
		//Try to create the bot's own camera
		headcam = scManager->addCameraSceneNode(NULL,getAbsPosition(),vector3df(0),9,false);
		if (headcam) {
			headcam->bindTargetAndRotation(true);
			camc_tries = 0;
		} else {
			camc_tries++;
			return NULL;
		}
	}
	if (botRoot) {
		headcam->setPosition(botRoot->getPosition());//+vector3df(0,mHeight,0));
//		headcam->setRotation(botRoot->getRotation());
//		headcam->setParent(botRoot);
	} else if (head) {
		head->GetRootSceneNode()->updateAbsolutePosition();
		headcam->setPosition(head->GetRootSceneNode()->getAbsolutePosition());
//		headcam->setRotation(head->GetRootSceneNode()->getRotation());
//		headcam->setParent(head->GetRootSceneNode());
	}
	return headcam;
}

void CGWIC_Bot::QuantumUpdate()
{
	if (master_bot) {
		mycell = master_bot->GetCell();
		SetPos(master_bot->GetPos());
//		if ((initParams.type == ACTOR_PLAYER) && (headcam)) {
//			std::cout << botRoot->getPosition().X << "  " << botRoot->getPosition().Y << std::endl;
//			std::cout << headcam->getPosition().X << "  " << headcam->getPosition().Y << std::endl;
//		}
	}
	//TODO: fix classic actors models rotations
	if (head) head->Quantum();
}

bool CGWIC_Bot::ProcessEvent(const irr::SEvent& event)
{
	if (!GetCamera()) return false;
	if (event.EventType == EET_MOUSE_INPUT_EVENT) {
		vector3df orot(headcam->getRotation());
		orot.X -= event.MouseInput.Y-mousepos.Y;
		orot.Y += event.MouseInput.X-mousepos.X;
		orot.Z = 0;
		headcam->setRotation(orot);
		mousepos.X = event.MouseInput.X;
		mousepos.Y = event.MouseInput.Y;
	} else if ((event.EventType == EET_KEY_INPUT_EVENT)) {// && (!event.KeyInput.PressedDown)) {
		QuantumUpdate();
		vector3df pos = GetPos();
//		pos.Y -= mHeight / 2;
		switch (event.KeyInput.Key) {
		case KEY_KEY_W: pos.X += 0.8f; break;
		case KEY_KEY_A: pos.Z += 0.8f; break;
		case KEY_KEY_S: pos.X -= 0.8f; break;
		case KEY_KEY_D: pos.Z -= 0.8f; break;
		default: return false;
		}
		if (master_bot) master_bot->SetPos(pos);
		else SetPos(pos);
		std::cout << pos.X << "  " << pos.Z << std::endl;
		return true;
	}
	return false;
}


}
