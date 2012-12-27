/*
 * CGWIC_BodyPart.h
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWIC_BODYPART_H_
#define CGWIC_BODYPART_H_

#include <iostream>
#include <stdio.h>
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
#include <GWICTypes.h>

namespace gwic {

struct GWIC_BPSlot {
	int ID;
	irr::core::vector3df posit;
	irr::core::vector3df rotat;
	irr::core::vector3df axis;
};
/*
struct GWIC_BPModel {
	irr::scene::IAnimatedMesh* mesh;
	ICollisionShape* coll;
	bool inuse;
};
*/

class CGWIC_BodyPart {
public:
	CGWIC_BodyPart(irr::io::path modelfile, irr::scene::ISceneNode* parentn, irr::IrrlichtDevice* dev, irrBulletWorld* phy);
	virtual ~CGWIC_BodyPart();
	void SetName(irr::core::stringc nwname);
	irr::core::stringc GetName() { return this->name; }
	bool Connect(CGWIC_BodyPart* part, int slotnum);
	CGWIC_BodyPart* GetNodeByName(irr::core::stringc nam);
	void SetScale(irr::core::vector3df scal, bool childs);
	irr::scene::ISceneNode* GetRootSceneNode() { return this->root; }
	irr::core::vector3df GetMaxExtent();
	void SetActive(bool activate, bool chain);
	bool GetActive() { return this->active; }
	void RebuildPhysics(bool chain);
	void UpdatePosition();
	void Move(irr::core::vector3df vec);
	irr::core::vector3df GetPos();
	IRigidBody* GetRigidBodyPtr() { return this->colbody; }
	GWIC_BPSlot GetSlotByID(int sid);
	GWIC_BPSlot GetSlotByChild(CGWIC_BodyPart* childptr);
	irr::s32 RecursiveSearchForNode(irr::scene::ISceneNode* nodeptr);
	CGWIC_BodyPart* GetBPbyNode(irr::scene::ISceneNode* nodeptr);
protected:
	bool success;
	bool active;
	irr::core::stringc name;
	irr::core::vector3df scale;
	std::vector<CGWIC_BodyPart*> slots;
	std::vector<GWIC_BPSlot> slot_outs;
	GWIC_BPSlot slot_in;
	irr::scene::IAnimatedMesh* mesh;
	irr::scene::IAnimatedMeshSceneNode* root;
	float mass;
	ICollisionShape* collider;
	irr::scene::IMesh* colmesh;
	ECollisionShapeType coltype;
	IRigidBody* colbody;
	btHingeConstraint* hinge;
	bool nocollision;
	irr::scene::ISceneNode* parent;
	irr::IrrlichtDevice* irDevice;
	irrBulletWorld* phy_world;
	irr::scene::ISceneManager* scManager;
	irr::video::IVideoDriver* irDriver;
	bool LoadModelFile(irr::io::path fname);
	void SlotsGrow(irr::u32 newsize);
};

}

#endif /* CGWIC_BODYPART_H_ */
