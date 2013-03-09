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
	virtual bool SetPos(irr::core::vector3df rel_pos);
	virtual irr::core::vector3df GetPos();
	virtual bool SetRot(irr::core::vector3df rot);
	virtual irr::core::vector3df GetRot();
	virtual void SetScale(irr::core::vector3df scal);
	virtual irr::core::vector3df GetScale();
	virtual CPoint2D GetCell();
	virtual void SetCell(CPoint2D ncell);
	virtual bool SetMaterial(ObjMaterial newmat);
	virtual ObjMaterial GetMaterial();
	virtual void SetEnabled(const bool enable);
	virtual bool GetEnabled() { return this->enabled; }
	virtual bool SetPhysical(const bool enable);
	virtual bool GetPhysical() { return this->physical; }
	virtual void SetVisible(const bool enable);
	virtual bool GetVisible() { return this->visible; }
	virtual irr::scene::ISceneNode* GetRootNode();
	virtual irr::core::vector3df getAbsPosition(irr::core::vector3df rel_pos);
	virtual irr::io::path GetFileName() { return this->lastfilename; }
	virtual void QuantumUpdate();
	bool isLight() { return false; }
	virtual bool SerializeToFile(irr::io::path filename);
private:
	CPoint2D mycell;
	irr::core::vector3df position;
	bool enabled;
	bool physical;
	bool was_physical;
	bool visible;
	irr::scene::ISceneNode* root;
	irr::scene::ISceneManager* scManager;
	irr::video::IVideoDriver* irDriver;
	irrBulletWorld* phy_world;
	ObjMaterial myMaterial;
	std::vector<ICollisionShape*> pshapes;
	std::vector<IRigidBody*> bodies;
	CGWIC_VM* cpu;
	irr::io::path lastfilename;
	void * objectInventory;
	void LoadXMLDescription(irr::io::path filename);
};

}

#endif /* CGWIC_GAMEOBJECT_H_ */
