/*
 * CGWIC_GameObject.h
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWIC_GAMEOBJECT_H_
#define CGWIC_GAMEOBJECT_H_

#include <iostream>
#include <string>
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
#include "CGWIC_VM.h"

namespace gwic {

class CGWIC_GameObject {
public:
	CGWIC_GameObject(irr::io::path filename, CPoint2D cell, irr::IrrlichtDevice* dev, irrBulletWorld* phy);
	virtual ~CGWIC_GameObject();
	bool SetPos(irr::core::vector3df rel_pos);
	irr::core::vector3df GetPos();
	bool SetRot(irr::core::vector3df rot);
	irr::core::vector3df GetRot();
	void SetScale(irr::core::vector3df scal);
	irr::core::vector3df GetScale();
	CPoint2D GetCell();
	void SetCell(CPoint2D ncell);
	bool SetMaterial(ObjMaterial newmat);
	ObjMaterial GetMaterial();
	void SetEnabled(bool enable);
	bool GetEnabled();
	bool SetPhysical(bool enable);
	bool GetPhysical();
	irr::scene::ISceneNode* GetRootNode();
	irr::core::vector3df getAbsPosition(irr::core::vector3df rel_pos);
private:
	CPoint2D mycell;
	irr::core::vector3df position;
	bool enabled;
	bool physical;
	bool was_physical;
	irr::scene::ISceneNode* root;
	irr::scene::ISceneManager* scManager;
	irr::video::IVideoDriver* irDriver;
	irrBulletWorld* phy_world;
	ObjMaterial myMaterial;
	std::vector<ICollisionShape*> pshapes;
	std::vector<IRigidBody*> bodies;
	CGWIC_VM* cpu;
};

}

#endif /* CGWIC_GAMEOBJECT_H_ */
