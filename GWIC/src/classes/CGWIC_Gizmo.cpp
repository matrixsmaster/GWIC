/*
 * CGWIC_Gizmo.cpp
 *  Created on: Dec 30, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWIC_Gizmo.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_Gizmo::CGWIC_Gizmo(irr::IrrlichtDevice* dev)
{
	irDevice = dev;
	InitVars();
	k_ctrl = k_shift = k_alt = false;
	CreateArrows();
}

CGWIC_Gizmo::CGWIC_Gizmo(irr::IrrlichtDevice* dev, const irr::core::vector3df pos)
{
	irDevice = dev;
	InitVars();
	k_ctrl = k_shift = k_alt = false;
	abspos = pos;
	CreateArrows();
}

void CGWIC_Gizmo::InitVars()
{
	curhandle = -1;
	oldhandle = -1;
	cscale = vector3df(1);
}

CGWIC_Gizmo::~CGWIC_Gizmo()
{
	while (handles.size()) {
		handles.back()->remove();
		handles.pop_back();
	}
}

void CGWIC_Gizmo::ProcessKeyEvent(const irr::SEvent& event)
{
	EKEY_CODE key = event.KeyInput.Key;
	k_shift = (((key==KEY_SHIFT) || (key==KEY_LSHIFT) || (key==KEY_RSHIFT)) && (event.KeyInput.PressedDown));
	k_ctrl = (((key==KEY_CONTROL) || (key==KEY_LCONTROL) || (key==KEY_RCONTROL)) && (event.KeyInput.PressedDown));
	k_alt = (((key==KEY_MENU) || (key==KEY_LMENU) || (key==KEY_RMENU)) && (event.KeyInput.PressedDown));
//	if (k_ctrl) std::cout << "Gizmo.Ctrl" << std::endl;
//	if (k_shift) std::cout << "Gizmo.Shift" << std::endl;
}

void CGWIC_Gizmo::ProcessRay(irr::core::line3d<irr::f32> ray)
{
	ISceneCollisionManager* pmgr = irDevice->getSceneManager()->getSceneCollisionManager();
	vector3df rayhit;
	triangle3df hit_triag;
	ISceneNode* node = pmgr->getSceneNodeAndCollisionPointFromRay(ray,rayhit,hit_triag,GWIC_GIZMO_MASK,0);
	if (node) {
		if (curhandle < 0) {
			for (u32 i=0; i<handles.size(); i++)
				if (handles[i] == node) {
					curhandle = static_cast<s32> (i);
					oldpos = abspos;
					break;
				}
		}
		if (curhandle >= 0) {
			if (oldhandle == curhandle) {
				float dist = oldhit.getDistanceFrom(rayhit);
//				std::cout << "Handle " << curhandle << " active. Dist: " << dist << std::endl;
				vector3df ndiff(0);
				switch (curhandle) {
				case 0:
					ndiff.X += (oldhit.X < rayhit.X)? dist:(-dist);
					break;
				case 1:
					ndiff.Y += (oldhit.Y < rayhit.Y)? dist:(-dist);
					break;
				case 2:
					ndiff.Z += (oldhit.Z < rayhit.Z)? dist:(-dist);
					break;
				}
				if (k_ctrl) {
					absrot += (ndiff * 10);
					NormRot();
				} else if (k_shift)
					cscale += vector3df(ndiff.dotProduct(vector3df(1)));
				else if (k_alt)
					cscale += ndiff;
				else
					abspos += ndiff;
			}
			oldhandle = curhandle;
			oldhit = rayhit;
//			std::cout << rayhit.X << ':' << rayhit.Y << ':' << rayhit.Z << std::endl;
		}
		UpdateCell();
		UpdateGizmo();
	}
}

void CGWIC_Gizmo::UpdateCameraPos(const irr::core::vector3df campos)
{
	float nscale = abspos.getDistanceFrom(campos) / GWIC_GIZMO_SIZE_DIVIDER;
	for (u32 i=0; i<handles.size(); i++)
		handles[i]->setScale(vector3df(nscale));
}

irr::core::vector3df CGWIC_Gizmo::GetCellRelativePosMetric()
{
	vector3df r = abspos / GWIC_IRRUNITS_PER_METER;
	r.X -= (cell.X * GWIC_METERS_PER_CELL);
	r.Z -= (cell.Y * GWIC_METERS_PER_CELL);
	return r;
}

irr::core::vector3df CGWIC_Gizmo::GetDifference()
{
	lastdiff = abspos - oldpos;
	oldpos = abspos;
	return lastdiff;
}

irr::core::vector3df CGWIC_Gizmo::GetDifferenceMetric()
{
	vector3df r = GetDifference() / GWIC_IRRUNITS_PER_METER;
	return r;
}

irr::core::vector3df CGWIC_Gizmo::GetRelativeScale()
{
	vector3df r(cscale);
	cscale = vector3df(1);
	return r;
}

void CGWIC_Gizmo::SetCellRelativePosMetric(irr::core::vector3df vec)
{
	vec.X += (cell.X * GWIC_METERS_PER_CELL);
	vec.Z += (cell.Y * GWIC_METERS_PER_CELL);
	abspos = vec * GWIC_IRRUNITS_PER_METER;
	UpdateGizmo();
}

void CGWIC_Gizmo::SetAbsolutePosition(const irr::core::vector3df vec)
{
	abspos = vec;
	UpdateCell();
	UpdateGizmo();
}

void CGWIC_Gizmo::UpdateCell()
{
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	cell.X = static_cast<int> (abspos.X/dim);
	cell.Y = static_cast<int> (abspos.Z/dim);
}

void CGWIC_Gizmo::CreateArrows()
{
	ISceneManager* smgr = irDevice->getSceneManager();
	IAnimatedMesh* arr;
	//X
	arr = smgr->addArrowMesh(L"X",GWIC_GIZMO_X_COLOR,GWIC_GIZMO_X_COLOR,GWIC_GIZMO_ARROW_PARAMS);
	vector3df rot(0,0,-90);
	IAnimatedMeshSceneNode* arrn = smgr->addAnimatedMeshSceneNode(arr,NULL,GWIC_GIZMO_MASK,abspos,rot);
	ITriangleSelector* sel = smgr->createTriangleSelector(arrn);
	arrn->setTriangleSelector(sel);
	sel->drop();
	handles.push_back(arrn);
	//Y
	arr = smgr->addArrowMesh(L"Y",GWIC_GIZMO_Y_COLOR,GWIC_GIZMO_Y_COLOR,GWIC_GIZMO_ARROW_PARAMS);
	rot = vector3df(0);
	arrn = smgr->addAnimatedMeshSceneNode(arr,NULL,GWIC_GIZMO_MASK,abspos,rot);
	sel = smgr->createTriangleSelector(arrn);
	arrn->setTriangleSelector(sel);
	sel->drop();
	handles.push_back(arrn);
	//Z
	arr = smgr->addArrowMesh(L"Z",GWIC_GIZMO_Z_COLOR,GWIC_GIZMO_Z_COLOR,GWIC_GIZMO_ARROW_PARAMS);
	rot = vector3df(90,0,0);
	arrn = smgr->addAnimatedMeshSceneNode(arr,NULL,GWIC_GIZMO_MASK,abspos,rot);
	sel = smgr->createTriangleSelector(arrn);
	arrn->setTriangleSelector(sel);
	sel->drop();
	handles.push_back(arrn);
	//apply material
	for (u32 i=0; i<handles.size(); i++) {
		handles[i]->setMaterialFlag(EMF_LIGHTING,false);
//		handles[i]->setMaterialFlag(EMF_ZWRITE_ENABLE,false);
	}
}

void CGWIC_Gizmo::UpdateGizmo()
{
	for (u32 i=0; i<handles.size(); i++)
		handles[i]->setPosition(abspos);
}

void CGWIC_Gizmo::GizmoHandleRelease()
{
	InitVars();
	UpdateCell();
	UpdateGizmo();
}

void CGWIC_Gizmo::NormRot()
{
	if (absrot.X < 360) absrot.X += 360.f;
	if (absrot.X > 360) absrot.X -= 360.f;
	if (absrot.Y < 360) absrot.Y += 360.f;
	if (absrot.Y > 360) absrot.Y -= 360.f;
	if (absrot.Z < 360) absrot.Z += 360.f;
	if (absrot.Z > 360) absrot.Z -= 360.f;
}


} /* namespace gwic */
