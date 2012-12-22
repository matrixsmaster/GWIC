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

namespace gwic {

enum GWICActorType {
	ACTOR_DUMMY,
	ACTOR_NPC,
	ACTOR_RC,
	ACTOR_PLAYER,
	ACTOR_VEHICLE,
	ACTOR_CREATURE
};

struct BotCreationParams {
	GWICActorType type;
	CPoint2D cell_coord;
	irr::core::vector3df rel_pos;
	float height;
	irr::core::stringw filename;
};

class CGWIC_Bot { //: public CGWIC_GameObject {
public:
	CGWIC_Bot(BotCreationParams* params, irr::IrrlichtDevice* dev, irrBulletWorld* phy);
	virtual ~CGWIC_Bot();
	bool SetPos(irr::core::vector3df rel_pos);
	irr::core::vector3df GetPos();
	bool SetCell(CPoint2D ncl);
	CPoint2D GetCell();
	irr::core::vector3df getAbsPosition();
	//irr::core::vector3df getAbsPosition(irr::core::vector3df rel_pos);
	bool SetRot(irr::core::vector3df rot);
	irr::core::vector3df GetRot();
	bool SetMaterial(ObjMaterial newmat);
	ObjMaterial GetMaterial();
	void SetEnabled(bool enable);
	bool GetEnabled();
	void AutoSize();
	irr::core::stringc GetTypeAsString();
protected:
	bool initDone;
	int ID;
	CPoint2D mycell;
	irr::core::vector3df position;
	bool enabled;
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
	float mHeight; // in irrUnits
	void LogIt(irr::core::stringc msg);
	CGWIC_Head* CreateNPC(irr::core::stringw file);
};

}

#endif /* CGWIC_BOT_H_ */
