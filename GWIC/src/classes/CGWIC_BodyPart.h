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
	//FIXME: make things more nice looking, sort it! +))
	virtual void SetName(irr::core::stringc nwname);
	virtual irr::core::stringc GetName() { return this->name; }
	virtual bool Connect(CGWIC_BodyPart* part, int slotnum);
	virtual CGWIC_BodyPart* GetNodeByName(irr::core::stringc nam);
	virtual void SetScale(irr::core::vector3df scal, bool childs);
	virtual irr::scene::ISceneNode* GetRootSceneNode() { return this->root; }
	virtual irr::core::vector3df GetMaxExtent();
	virtual void SetActive(bool activate, bool chain);
	virtual bool GetActive() { return this->active; }
	virtual void SetVisible(const bool enable);
	virtual bool GetVisible() { return this->visible; }
	virtual void RebuildPhysics(bool chain);
	virtual void UpdatePosition();
	virtual void Move(irr::core::vector3df vec);
	virtual irr::core::vector3df GetAbsPos();
	virtual IRigidBody* GetRigidBodyPtr() { return this->colbody; }
	virtual GWIC_BPSlot GetSlotByID(int sid);
	virtual GWIC_BPSlot GetSlotByChild(CGWIC_BodyPart* childptr);
	virtual irr::s32 RecursiveSearchForNode(irr::scene::ISceneNode* nodeptr);
	virtual CGWIC_BodyPart* GetBPbyNode(irr::scene::ISceneNode* nodeptr);
	virtual irr::core::vector3df GetRealPosition();
	virtual bool ApplyRotationForce(irr::core::vector3df rotvec) { return true; }
	virtual bool SerializeToFile(irr::io::path filename);
protected:
	bool success;
	bool active;
	bool visible;
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
