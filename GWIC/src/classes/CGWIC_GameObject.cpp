/*
 * CGWIC_GameObject.cpp
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWIC_GameObject.h"
#include "CGWIC_Cell.h"
//#include "CGWIC_InventoryObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_GameObject::CGWIC_GameObject(irr::io::path filename, CPoint2D cell, irr::IrrlichtDevice* dev, irrBulletWorld* phy) {
	std::cout << "new GameObject created" << std::endl;
	scManager = dev->getSceneManager();
	irDriver = dev->getVideoDriver();
	root = NULL;
	phy_world = phy;
	enabled = false;
	physical = false;
	was_physical = false;
	visible = true;
	mycell = cell;
	cpu = NULL;
	if (filename.size() < 4) return;
	if (filename.subString(filename.size()-3,3,true).equalsn(L"xml",3))
		LoadXMLDescription(GWIC_OBJECTS_DIR+filename);
	else {
		//load static mesh
		IMesh* rtmesh = scManager->getMesh(GWIC_STATMESHES_DIR+filename);
		root = scManager->addMeshSceneNode(rtmesh,NULL,GWIC_PICKABLE_MASK);
		if (!root) {
			std::cerr << "Unable to load static mesh " << filename.c_str() << std::endl;
			return;
		}
		std::cout << "Static mesh created" << std::endl;
		ITriangleSelector* sel = scManager->createTriangleSelector(rtmesh,root);
		root->setTriangleSelector(sel);
		sel->drop();
		//root->setScale(vector3df(GWIC_IRRUNITS_PER_METER));
		pshapes.push_back(new IBoxShape(root,0,false));
	}
}

CGWIC_GameObject::~CGWIC_GameObject()
{
	std::cout << "Removing object data... ";
	if (physical) SetPhysical(false);
	while (pshapes.size() > 0) {
		//pshapes.back()->removeNode();
		delete (pshapes.back());
		pshapes.pop_back();
	}
	pshapes.clear();
	if (root) root->remove();
	std::cout << "done!" << std::endl;
}

bool CGWIC_GameObject::SetPos(irr::core::vector3df rel_pos)
{
	if (!root) return false;
	position = rel_pos;
	vector3df oldpos = root->getPosition();
	root->setPosition(getAbsPosition(rel_pos));
	if (physical) {
		for (u32 i=0; i<bodies.size(); i++) {
			btTransform btt = bodies[i]->getPointer()->getCenterOfMassTransform();
			btVector3 btv = irrlichtToBulletVector(root->getPosition()-oldpos);
			btt.setOrigin(btt.getOrigin() + btv);
			bodies[i]->getPointer()->setCenterOfMassTransform(btt);
			if (bodies[i]->getActivationState() == EAS_SLEEPING)
				bodies[i]->setActivationState(EAS_ACTIVE);
		}
	}
	return true;
}

irr::core::vector3df CGWIC_GameObject::GetPos()
{
	if (!physical) return this->position;
	vector3df out(-1.f);
	if (!root) return out;
	out = root->getPosition();
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	out.X -= (dim * mycell.X);
	out.Z -= (dim * mycell.Y);
	out /= GWIC_IRRUNITS_PER_METER;
	return out;
}

bool CGWIC_GameObject::SetRot(irr::core::vector3df rot)
{
	if (root) {
		root->setRotation(rot);
		if (physical) {
			btTransform bto,btn;
			btScalar rx = rot.X / PI * 180.f;
			btScalar ry = rot.Y / PI * 180.f;
			btScalar rz = rot.Z / PI * 180.f;
			btQuaternion btq;
			btq.setEulerZYX(ry,rx,rz);
			bto.setIdentity();
			bto.setRotation(btq);
			for (u32 i=0; i<bodies.size(); i++) {
				btn = bodies[i]->getPointer()->getCenterOfMassTransform();
				bto.setOrigin(btn.getOrigin());
				bodies[i]->getPointer()->setCenterOfMassTransform(bto);
				if (bodies[i]->getActivationState() == EAS_SLEEPING)
					bodies[i]->setActivationState(EAS_ACTIVE);
			}
		}
		return true;
	}
	return false;
}

irr::core::vector3df CGWIC_GameObject::GetRot()
{
	if (root) return root->getRotation();
	else return (vector3df(0));
}

void CGWIC_GameObject::SetScale(irr::core::vector3df scal)
{
	if (!root) return;
	root->setScale(scal);
	for (u32 i=0; i<pshapes.size(); i++) {
		pshapes[i]->setLocalScaling(scal,ESP_COLLISIONSHAPE);
	}
}

irr::core::vector3df CGWIC_GameObject::GetScale()
{
	return (this->root->getScale());
}

CPoint2D CGWIC_GameObject::GetCell()
{
	return this->mycell;
}

void CGWIC_GameObject::SetCell(CPoint2D ncell)
{
	mycell = ncell;
}

bool CGWIC_GameObject::SetMaterial(ObjMaterial newmat)
{
	//
	return true;
}

ObjMaterial CGWIC_GameObject::GetMaterial()
{
	return this->myMaterial;
}

void CGWIC_GameObject::SetEnabled(const bool enable)
{
	if (enabled == enable) return;
	if (enable) {
		//...
		if (was_physical) SetPhysical(true);
	} else {
		if (physical) {
			was_physical = true;
			SetPhysical(false);
		}
	}
	enabled = enable;
}

bool CGWIC_GameObject::SetPhysical(const bool enable)
{
	if (physical == enable) return true;
	if (enable) {
		size_t i;
		for (i=0; i<pshapes.size(); i++)
			bodies.push_back(phy_world->addRigidBody(pshapes[i]));
	} else {
		while (bodies.size() > 0) {
			//phy_world->addToDeletionQueue(bodies.back());
			phy_world->removeCollisionObject(bodies.back(),true);
			//bodies.back()->remove();
			//delete (bodies.back());
			bodies.pop_back();
		}
	}
	physical = enable;
	return true;
}

void CGWIC_GameObject::SetVisible(const bool enable)
{
	//Note: don't use this function from within object's own methods!
	if ((enabled) && (root)) {
		visible = enable;
		root->setVisible(enable);
	}
}

irr::scene::ISceneNode* CGWIC_GameObject::GetRootNode()
{
	return this->root;
}

irr::core::vector3df CGWIC_GameObject::getAbsPosition(irr::core::vector3df rel_pos)
{
	vector3df res(rel_pos);
	res *= GWIC_IRRUNITS_PER_METER;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	res.X += (dim * mycell.X);
	res.Z += (dim * mycell.Y);
	return res;
}

void CGWIC_GameObject::LoadXMLDescription(irr::io::path filename)
{
	std::cout << "Object has XML description!" << std::endl;
	//TODO: loading xml
}

void CGWIC_GameObject::QuantumUpdate()
{
	//TODO: update cpu, state, etc
}


}
