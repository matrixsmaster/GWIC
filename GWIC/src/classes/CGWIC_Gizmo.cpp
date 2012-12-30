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
	CreateArrows();
}

CGWIC_Gizmo::CGWIC_Gizmo(irr::IrrlichtDevice* dev, irr::core::vector3df pos)
{
	irDevice = dev;
	abspos = pos;
	CreateArrows();
}

CGWIC_Gizmo::~CGWIC_Gizmo()
{
	while (handles.size()) {
		handles.back()->remove();
		handles.pop_back();
	}
}

void CGWIC_Gizmo::ProcessEvent(const irr::SEvent& event)
{
	//
}

irr::core::vector3df CGWIC_Gizmo::GetCellRelativePosMetric()
{
	vector3df r = abspos / GWIC_IRRUNITS_PER_METER;
	r.X -= (cell.X * GWIC_METERS_PER_CELL);
	r.Z -= (cell.Y * GWIC_METERS_PER_CELL);
	return r;
}

void CGWIC_Gizmo::SetCellRelativePosMetric(irr::core::vector3df vec)
{
	vec.X += (cell.X * GWIC_METERS_PER_CELL);
	vec.Z += (cell.Y * GWIC_METERS_PER_CELL);
	abspos = vec * GWIC_IRRUNITS_PER_METER;
}

void CGWIC_Gizmo::SetAbsolutePosition(const irr::core::vector3df vec)
{
	abspos = vec;
	UpdateCell();
}

void CGWIC_Gizmo::UpdateCell()
{
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	cell.X = static_cast<int> (abspos.X/dim);
	cell.Y = static_cast<int> (abspos.Z/dim);
}

void CGWIC_Gizmo::CreateArrows()
{
	//
}


} /* namespace gwic */
