/*
 * CGWIC_BodyPart.cpp
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWIC_BodyPart.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

namespace gwic {

CGWIC_BodyPart::CGWIC_BodyPart(irr::io::path modelfile, irr::scene::ISceneNode* parentn, irr::IrrlichtDevice* dev, irrBulletWorld* phy)
{
	std::cout << "BodyPart[] c'tor()" << std::endl;
	std::cout << "modelfile: " << modelfile.c_str() << std::endl;
	irDevice = dev;
	phy_world = phy;
	scManager = dev->getSceneManager();
	irDriver = dev->getVideoDriver();
	parent = parentn;
	mesh = NULL;
	root = NULL;
	mass = 2.f;
	collider = NULL;
	colmesh = NULL;
	colbody = NULL;
	hinge = NULL;
	slots.push_back(NULL); //zero slot is in-slot
	success = LoadModelFile(modelfile);
	active = false;
}

CGWIC_BodyPart::~CGWIC_BodyPart() {
	std::cout << "BodyPart[] destructor called!" << std::endl;
	if (active) SetActive(false,false);
	if (collider) delete collider;
	//delete slots except zero
	while (slots.size() > 1) {
		if (slots.back()) delete (slots.back());
		slots.pop_back();
	}
	std::cout << name.c_str() << " destroyed" << std::endl;
	//FIXME: destroy colmesh if it wasn't used or some
}

void CGWIC_BodyPart::SetName(irr::core::stringc nwname)
{
	this->name = nwname;
}

bool CGWIC_BodyPart::Connect(CGWIC_BodyPart* part, int slotnum)
{
	if (!part) return false;
	if (slotnum < 0) {
		//TODO: maybe doing some things like wildcard assign or something
		//i'll do it later ;)
		return false;
	} else {
		u32 n = static_cast<u32> (slotnum);
		if (n >= slots.size()) SlotsGrow(n+1);
		slots[n] = part;
		return true;
	}
}

CGWIC_BodyPart* CGWIC_BodyPart::GetNodeByName(irr::core::stringc nam)
{
	if (nam == this->name) return this;
	CGWIC_BodyPart* ptr;
	for (u32 i=1; i<slots.size(); i++) {
		if (!slots[i]) continue;
		ptr = slots[i]->GetNodeByName(nam);
		if (ptr) return ptr;
	}
	return NULL;
}

void CGWIC_BodyPart::SetScale(irr::core::vector3df scal, bool childs)
{
	this->scale = scal;
	if (!root) {
		std::cerr << "Setting scale on object " << name.c_str() << " without the scene node!" << std::endl;
		return;
	}
	//FIXME: scale the object ;)
//	boundbox bx1 = root->getBoundingBox();
	if (collider) collider->setLocalScaling(scal,ESP_BOTH);
	else if (root) root->setScale(scal);
//	boundbox bx2 = root->getBoundingBox();
	if (childs) {
		for (u32 i=1; i<slots.size(); i++)
			if (slots[i]) slots[i]->SetScale(scal,true);
	}
}

irr::scene::ISceneNode* CGWIC_BodyPart::GetRootSceneNode()
{
	return this->root;
}

irr::core::vector3df CGWIC_BodyPart::GetMaxExtent()
{
	if (!root) return (vector3df(0));
	boundbox bx = this->root->getBoundingBox();
	vector3df ext = bx.getExtent();
	for (u32 i=1; i<slots.size(); i++) {
		if (!slots[i]) continue;
		if (slots[i]->GetRootSceneNode()) {
			bx = slots[i]->GetRootSceneNode()->getBoundingBox();
			if (bx.getExtent().X > ext.X) ext.X = bx.getExtent().X;
			if (bx.getExtent().Y > ext.Y) ext.Y = bx.getExtent().Y;
			if (bx.getExtent().Z > ext.Z) ext.Z = bx.getExtent().Z;
		}
	}
	return ext;
}

void CGWIC_BodyPart::SetActive(bool activate, bool chain)
{
	if (!success) return;
	//if (activate == active) return;
	if (activate) {
		if (collider) colbody = phy_world->addRigidBody(collider);
		if (!colbody)
			std::cerr << "colbody is NULL during bodypart " << name.c_str() << " activation!!!" << std::endl;
		UpdatePosition();
		if ((colbody) && (slots[0]) && (slots[0]->GetRigidBodyPtr())) {
			std::cout << "Creating slot's hinge constraint!" << std::endl;
			btRigidBody* pt1 = slots[0]->GetRigidBodyPtr()->getPointer();
			btRigidBody* pt2 = colbody->getPointer();
			GWIC_BPSlot parslot = slots[0]->GetSlotByChild(this);
	//		GWIC_BPSlot ourslot = slot_in;
			btVector3 mypivotA(irrlichtToBulletVector(parslot.posit*scale));
			btVector3 myaxisA(irrlichtToBulletVector(parslot.axis*scale));
			btVector3 mypivotB(irrlichtToBulletVector(slot_in.posit*scale));
			btVector3 myaxisB(irrlichtToBulletVector(slot_in.axis*scale));
			hinge = new btHingeConstraint(*pt1,*pt2,mypivotA,mypivotB,myaxisA,myaxisB);
		}
		if ((colbody) && (hinge)) {
			std::cout << "enabling slot hinge" << std::endl;
			phy_world->getPointer()->addConstraint(hinge,true);
		}
	} else {
		if (hinge) {
			phy_world->getPointer()->removeConstraint(hinge);
//			delete hinge;
		}
		if (colbody) phy_world->removeCollisionObject(colbody,true);
	}
	if (chain) {
		for (u32 i=1; i<slots.size(); i++)
			if (slots[i]) slots[i]->SetActive(activate,true);
	}
	active = activate;
}

bool CGWIC_BodyPart::GetActive()
{
	return this->active;
}

void CGWIC_BodyPart::RebuildPhysics(bool chain)
{
	if ((nocollision) || (!root) ) return; //|| (!colmesh)
	//destroy old
//	SetActive(false,true);
//	if (hinge) delete hinge;
	if (colbody) phy_world->removeCollisionObject(colbody,true);
	if (collider) delete collider;
	collider = NULL;
	//create new
	switch (coltype) {
	case ECST_BOX:
		collider = new IBoxShape(root,mass,false);
		break;
	case ECST_SPHERE:
		collider = new ISphereShape(root,mass,false);
		break;
	case ECST_BVHTRIMESH:
	case ECST_CONVEXHULL:
	case ECST_GIMPACT:
	case ECST_CONE:
		std::cerr << "This shape isn't yet implemented!" << std::endl;
		break;
	}
	collider->setLocalScaling(root->getScale(),ESP_COLLISIONSHAPE);
	SetActive(true,false);
	//daisy chain
	if (chain) {
		for (u32 i=1; i<slots.size(); i++)
			if (slots[i]) slots[i]->RebuildPhysics(true);
	}
}

void CGWIC_BodyPart::UpdatePosition()
{
	if ((!root) || (!collider) || (!colbody)) return;
	root->updateAbsolutePosition();
	colbody->updateObject();
	btTransform btt = colbody->getPointer()->getCenterOfMassTransform();
	btt.setOrigin(irrlichtToBulletVector(root->getPosition()));
	colbody->getPointer()->setCenterOfMassTransform(btt);
}

void CGWIC_BodyPart::Move(irr::core::vector3df vec)
{
	root->setPosition(root->getPosition()+vec);
	UpdatePosition();
	for (u32 i=1; i<slots.size(); i++)
		if (slots[i]) slots[i]->Move(vec);
}

irr::core::vector3df CGWIC_BodyPart::GetPos()
{
	return this->root->getPosition();
}

bool CGWIC_BodyPart::LoadModelFile(irr::io::path fname)
{
	IXMLReader* mio = irDevice->getFileSystem()->createXMLReader(fname);
	if (!mio) return false;
	std::cout << "Reading model XML: " << fname.c_str() << std::endl;
	stringw mt_model(L"Model");
	stringw mt_inslot(L"InSlot");
	stringw mt_outslot(L"OutSlot");
	stringw mt_collision(L"Collision");
	stringw mt_colbox(L"StandardBox");
	stringw mt_colsph(L"StandardSphere");
	stringw mt_coltri(L"TriMeshShape");
	stringw mt_colcvx(L"ConvexHull");
	stringw mt_colgim(L"GImpact");
	GWIC_BPSlot cslot;
	CIrrStrParser strparse;
	nocollision = true; //in case we can't load or found collision shape model
	while (mio->read()) {
		if (mt_model.equals_ignore_case(mio->getNodeName())) {
			mesh = scManager->getMesh(mio->getAttributeValueSafe(L"file"));
			if (mesh) root = scManager->addAnimatedMeshSceneNode(mesh,parent,GWIC_PICKABLE_MASK);
			if (root) {
				root->updateAbsolutePosition();
				root->setMaterialFlag(EMF_NORMALIZE_NORMALS,true);
				ITriangleSelector* sel = scManager->createTriangleSelector(mesh,root);
				root->setTriangleSelector(sel);
				sel->drop();
			}
		} else if (mt_inslot.equals_ignore_case(mio->getNodeName())) {
			strparse = mio->getAttributeValueSafe(L"position");
			slot_in.posit = strparse.ToVector3f();
			strparse = mio->getAttributeValueSafe(L"rotation");
			slot_in.rotat = strparse.ToVector3f();
			strparse = mio->getAttributeValueSafe(L"axis");
			slot_in.axis = strparse.ToVector3f();
		} else if (mt_outslot.equals_ignore_case(mio->getNodeName())) {
			cslot.ID = mio->getAttributeValueAsInt(L"ID");
			strparse = mio->getAttributeValueSafe(L"position");
			cslot.posit = strparse.ToVector3f();
			strparse = mio->getAttributeValueSafe(L"rotation");
			cslot.rotat = strparse.ToVector3f();
			strparse = mio->getAttributeValueSafe(L"axis");
			cslot.axis = strparse.ToVector3f();
			slot_outs.push_back(cslot);
		} else if (mt_collision.equals_substring_ignore_case(mio->getNodeName())) {
			mass = mio->getAttributeValueAsFloat(L"mass");
			stringw data = mio->getAttributeValueSafe(L"type");
			nocollision = false;
			if (data.equals_substring_ignore_case(mt_colbox))
				coltype = ECST_BOX;
			else if (data.equals_substring_ignore_case(mt_colsph))
				coltype = ECST_SPHERE;
			else if (data.equals_substring_ignore_case(mt_coltri))
				coltype = ECST_BVHTRIMESH;
			else if (data.equals_substring_ignore_case(mt_colcvx))
				coltype = ECST_CONVEXHULL;
			else if (data.equals_substring_ignore_case(mt_colgim))
				coltype = ECST_GIMPACT;
			else {
				std::cerr << "Collision model unknown: " << data.c_str() << std::endl;
				nocollision = true;
			}
			//colmesh = scManager->getMesh(mio->getAttributeValueSafe(L"file"));
		}
	}
	mio->drop();
	return true;
}

/*
 * SlotsGrow() used to resize slots[] with filling all uninitialized elements with NULL
 * slots.reserve() can't return NULL-initialized array with guarantee on all platforms/compilers
 */
void CGWIC_BodyPart::SlotsGrow(irr::u32 newsize)
{
	while (slots.size() < newsize) slots.push_back(NULL);
}

IRigidBody* CGWIC_BodyPart::GetRigidBodyPtr()
{
	return this->colbody;
}

GWIC_BPSlot CGWIC_BodyPart::GetSlotByID(int sid)
{
	GWIC_BPSlot out;
	if (sid < 0) return out;
	if (sid == 0) out = slot_in;
	else {
		for (u32 i=0; i<slot_outs.size(); i++) {
			out = slot_outs[i];
			if (out.ID == sid) return out;
		}
	}
	out = GWIC_BPSlot();
	return out;
}

GWIC_BPSlot CGWIC_BodyPart::GetSlotByChild(CGWIC_BodyPart* childptr)
{
	GWIC_BPSlot out;
	if (!childptr) return out;
	for (u32 i=0; i<slots.size(); i++) {
		if (slots[i] == childptr) {
			//out position in slots<> is our ID
			return (GetSlotByID(static_cast<u32>(i)));
		}
	}
	out = GWIC_BPSlot();
	return out;
}


}

