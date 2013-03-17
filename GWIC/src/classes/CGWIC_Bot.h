/*
 * CGWIC_Bot.h
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWIC_BOT_H_
#define CGWIC_BOT_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <irrlicht.h>
#include <irrbullet.h>
#include <bulletworld.h>
#include <rigidbody.h>
#include <boxshape.h>
#include <sphereshape.h>
#include <gimpactmeshshape.h>
#include <raycastvehicle.h>
#include <collisionobjectaffectordelete.h>
#include <collisionobjectaffectorattract.h>
//#include <collisionobjectaffectorbuoyancy.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <OALWrapper/OAL_Types.h>
#include <OALWrapper/OAL_Funcs.h>
#include <GWICTypes.h>
#include "CGWIC_GameObject.h"
#include "CGWIC_Head.h"
#include "CGWIC_BodyPart.h"
#include "CGWIC_InventoryObject.h"
#include "CGWIC_Inventory.h"
#include "GWIC_SLAvatar.h"

namespace gwic {

enum GWICActorType {
	ACTOR_DUMMY,		//test actor type, loads Sydney from Irrlicht's package
	ACTOR_GYNOID,		//our new actor type - gynoid (or other actor) sliced to BPs
	ACTOR_PLAYER,		//dummy type for player's character
	ACTOR_CREATURE,		//classic "skinned" actor without bodyparts
	ACTOR_SLAVATAR		//SecondLife(tm) avatar, handles SLA skeleton + txd overlays & attachments + BVH anims
};

struct BotCreationParams {
	GWICActorType type;
	CPoint2D cell_coord;
	irr::core::vector3df rel_pos;
	float height;
	irr::io::path filename;
	irr::core::stringw actorname;
};

class CGWIC_Bot { //: public CGWIC_GameObject {
public:
	CGWIC_Bot(BotCreationParams* params, irr::IrrlichtDevice* dev, irrBulletWorld* phy);
	virtual ~CGWIC_Bot();
	//TODO: do some sorting and comment on this methods prototypes ;-)
	bool SetPos(irr::core::vector3df rel_pos);
	irr::core::vector3df GetPos();
	bool SetCell(CPoint2D ncl);
	CPoint2D GetCell() { return this->mycell; }
	irr::core::vector3df getAbsPosition();
	//irr::core::vector3df getAbsPosition(irr::core::vector3df rel_pos);
	float GetRealHeight() { return this->mHeight; }
	float GetMetricHeight() { return (this->mHeight/GWIC_IRRUNITS_PER_METER); }
	bool SetRot(irr::core::vector3df rot);
	irr::core::vector3df GetRot();
	bool SetMaterial(ObjMaterial newmat);
	ObjMaterial GetMaterial();
	void SetEnabled(bool enable);
	bool GetEnabled();
	void SetVisible(const bool enable);
	bool GetVisible() { return this->visible; }
	void AutoSize();
	GWICActorType GetType() { return this->initParams.type; }
	irr::core::stringc GetTypeAsString();
	irr::s32 IsThisNodeIsMine(irr::scene::ISceneNode* node);
	irr::core::stringw GetName() { return this->myname; }
	CGWIC_Head* GetHead() { return this->head; }
	void SetMaster(CGWIC_Bot* nwmaster);
	CGWIC_Bot* GetMaster() { return this->master_bot; }
	irr::scene::ICameraSceneNode* GetCamera();
	void QuantumUpdate();
	bool ProcessEvent(const irr::SEvent& event);
	bool isCompletelyDead();
	void AssignInventory(CGWIC_Inventory* newinv);
	CGWIC_Inventory* GetInventory() { return this->inventory; }
protected:
	//FIXME: sort it out!!!
	bool initDone;
	int ID;
	CPoint2D mycell;
	irr::core::vector3df position;
	bool enabled;
	bool visible;
	BotCreationParams initParams;
	irr::IrrlichtDevice* irDevice;
	irr::scene::ISceneManager* scManager;
	irr::video::IVideoDriver* irDriver;
	irrBulletWorld* phy_world;
	irr::scene::ISceneNode* botRoot;
	IRigidBody* botShell;
	ICollisionShape* basicShell;
	CGWIC_Head* head;
	ObjMaterial botmat;
	CGWIC_Inventory* inventory;
	irr::core::stringw myname;
	float mHeight; // in irrUnits
	CGWIC_Bot* master_bot;
	irr::scene::ICameraSceneNode* headcam;
	int camc_tries;
	CPoint2D mousepos;
	GWIC_SLAvatar* slAvatar;
	void LogIt(irr::core::stringc msg);
	CGWIC_Head* CreateNPC(irr::io::path file);
};

}

#endif /* CGWIC_BOT_H_ */
