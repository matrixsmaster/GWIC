/*
 * CGWIC_Bot.cpp
 *  Created on: Dec 3, 2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012-2013
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
		botmesh = scManager->getMesh(GWIC_ACTORS_DIR+"sydney.md2");
		if (!botmesh) return;
		animnode = scManager->addAnimatedMeshSceneNode(botmesh,NULL,
				GWIC_ACTOR_MASK | GWIC_PICKABLE_MASK);
		botRoot = animnode;
		botRoot->setMaterialTexture(0,irDriver->getTexture(GWIC_ACTORS_DIR+"sydney.bmp"));
//		animnode->addShadowVolumeSceneNode();
		animnode->setMD2Animation(EMAT_STAND);
		basicShell = new IBoxShape(botRoot,60.f,false);
		break;
	case ACTOR_GYNOID:
		head = CreateNPC(GWIC_ACTORS_DIR+params->filename);
		if (head) SetEnabled(true);
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
	case ACTOR_SLAVATAR:
		/*
		 * SecondLife(tm) / OpenSim avatar-like actor.
		 * This type of actor represents geometry only, physics describes by a simple
		 * capsule, just like ACTOR_CREATURE
		 */
		slAvatar = new GWIC_SLAvatar(GWIC_ACTORS_DIR+params->filename,scManager);
		break;
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
//	if (slAvatar) slAvatar->remove();
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
		position.Y -= GetMetricHeight() / 2;
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

/*
 * Return true if rotating successful, or false if there're some problems
 * or just rotation limit reached.
 */
bool CGWIC_Bot::SetRot(irr::core::vector3df rot)
{
	if (GetType() != ACTOR_GYNOID) {
		rot.X = 0; rot.Z = 0;
	}
	if (botRoot) {
		botRoot->setRotation(rot);
		if (botShell) {
			//FIXME
		}
	}
	if (head) return (head->ApplyRotationForce(rot));
	return true;
}

irr::core::vector3df CGWIC_Bot::GetRot()
{
	if (head) return (head->GetRootSceneNode()->getRotation());
	if (botRoot) return (botRoot->getRotation());
	return (vector3df(0));
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

CGWIC_Head* CGWIC_Bot::CreateNPC(irr::io::path file)
{
	/* There was strange error: if we create root node and then move it, geometry moves too,
	 * but boundbox and absolute position remains the same, even with updateAbsolutePosition()
	 * calls, so I've decided to not to handle entire bot structure as Irrlicht's hierarchy
	 * and use our internal GWIC classes facilities to handle the tree, move it, scale, hide, etc
	 */
	CGWIC_Head* nhead = NULL;
	IXMLReader* xml = irDevice->getFileSystem()->createXMLReader(file);
	if (!xml) {
		std::cerr << "CreateNPC(): xml read failed for file " << file.c_str() << std::endl;
		return NULL;
	}
	const stringw se_head = L"HEAD";
	const stringw se_bpart = L"Bodypart";
	const stringw se_conn = L"Connect";
	std::vector<CGWIC_BodyPart*> parts;
	CGWIC_BodyPart* cpart, *cprt2;
	stringw bpname,childn;
	while (xml->read()) {
		if ((!nhead) && (se_head.equals_ignore_case(xml->getNodeName()))) {
			nhead = new CGWIC_Head(xml->getAttributeValueSafe(L"filename"),botRoot,irDevice,phy_world);
			if (!nhead) std::cerr << "CreateNPC(): Failed to load bot's HEAD" << std::endl;
			else parts.push_back(nhead);
		} else if (se_bpart.equals_ignore_case(xml->getNodeName())) {
			cpart = new CGWIC_BodyPart(xml->getAttributeValueSafe(L"filename"),botRoot,irDevice,phy_world);
			bpname = xml->getAttributeValueSafe(L"name");
			if (!cpart) std::cerr << "CreateNPC(): Failed to load bot's BP " << stringc(bpname).c_str() << std::endl;
			else {
				cpart->SetName(bpname);
				parts.push_back(cpart);
			}
		} else if (se_conn.equals_ignore_case(xml->getNodeName())) {
			bpname = xml->getAttributeValueSafe(L"parent_name");
			childn = xml->getAttributeValueSafe(L"child_name");
			cpart = cprt2 = NULL;
			for (u32 i=0; i<parts.size(); i++) {
				if (parts[i]->GetName() == bpname) cpart = parts[i];
				else if (parts[i]->GetName() == childn) cprt2 = parts[i];
				if ((cpart) && (cprt2)) break;
			}
			if ((!cpart) || (!cprt2))
				std::cerr << "CreateNPC(): Connect failed. One of nodes isn't found!" << std::endl;
			else {
				if ((cpart->Connect(cprt2,xml->getAttributeValueAsInt(L"parent_slot"))) &&
						(cprt2->Connect(cpart,0))) {
					std::cout << "CreateNPC(): Connect success" << std::endl;
				} else
					std::cerr << "CreateNPC(): Connect failed. Unknown error!" << std::endl;
			}
		}
	}
	if (!nhead) {
		std::cerr << "CreateNPC(): None of HEADs declared in xml loaded. Destroying loaded BPs." << std::endl;
		while (parts.size()) {
			delete (parts.back());
			parts.pop_back();
		}
	}// else nhead->SetActive(true,true);
	/*
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
	*/
	xml->drop();
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
	//FIXME: in normal situation, slave bot's camera is equal to master's
//	if ((master_bot) && (master_bot->GetCamera()))
//		return (master_bot->GetCamera());
	if (!headcam) {
		if (camc_tries > 3) return NULL;
		//Try to create the bot's own camera
		headcam = scManager->addCameraSceneNode(NULL,getAbsPosition(),vector3df(0),9,false);
		if (botRoot) {
			headcam->setRotation(botRoot->getRotation());
//			headcam->setParent(botRoot);
//			botRoot->addChild(headcam);
		} else if (head) {
			headcam->setRotation(head->GetRootSceneNode()->getRotation());
//			headcam->setParent(head->GetRootSceneNode());
//			head->GetRootSceneNode()->addChild(headcam);
		}
		if (headcam) {
			headcam->bindTargetAndRotation(true);
			camc_tries = 0;
		} else {
			camc_tries++;
			return NULL;
		}
	}
	if (botRoot) {
		headcam->setPosition(botRoot->getPosition()+vector3df(0,mHeight,0));
	} else if (head) {
		head->GetRootSceneNode()->updateAbsolutePosition();
		headcam->setPosition(head->GetRootSceneNode()->getAbsolutePosition());
		headcam->setRotation(head->GetStraightLook());
	}
	return headcam;
}

void CGWIC_Bot::QuantumUpdate()
{
	if (master_bot) {
		mycell = master_bot->GetCell();
		vector3df hshift(0,master_bot->GetMetricHeight(),0);
		if (master_bot->GetType() == ACTOR_GYNOID)
			hshift.Y /= 2;
		SetPos(master_bot->GetPos()+hshift);
		SetRot(master_bot->GetRot());
	} else
		SetRot(GetRot());
	if (head) head->Quantum();
}

bool CGWIC_Bot::ProcessEvent(const irr::SEvent& event)
{
	if (GetType() != ACTOR_PLAYER) return false;
	if (!GetCamera()) return false;
	if (event.EventType == EET_MOUSE_INPUT_EVENT) {
		vector3df orot(headcam->getRotation());
		orot.X -= (event.MouseInput.Y-mousepos.Y) / 2;
		orot.Y += (event.MouseInput.X-mousepos.X) / 2;
		orot.Z = 0;
		if (master_bot) {
			master_bot->SetRot(orot);
			if (master_bot->GetType() == ACTOR_GYNOID) {
				if (master_bot->GetHead())
					orot = master_bot->GetHead()->GetStraightLook();
				else
					orot = master_bot->GetRot();
			}
			headcam->setRotation(orot);
		}
		mousepos.X = event.MouseInput.X;
		mousepos.Y = event.MouseInput.Y;
	} else if ((event.EventType == EET_KEY_INPUT_EVENT)) {// && (!event.KeyInput.PressedDown)) {
		QuantumUpdate();
		if (master_bot) {
			vector3df shf(0);
			switch (event.KeyInput.Key) {
			case KEY_KEY_W: shf.X += 0.8f; break;
			case KEY_KEY_A: shf.Z += 0.8f; break;
			case KEY_KEY_S: shf.X -= 0.8f; break;
			case KEY_KEY_D: shf.Z -= 0.8f; break;
			default: return false;
			}
			shf = master_bot->GetRot().rotationToDirection(shf);
			master_bot->SetPos(master_bot->GetPos()+shf);
		}
		return true;
	}
	return false;
}

bool CGWIC_Bot::isCompletelyDead()
{
	return ( (GetType()!=ACTOR_PLAYER) &&
			(!botRoot) &&
			(!head) &&
			(!slAvatar) );
}


}
