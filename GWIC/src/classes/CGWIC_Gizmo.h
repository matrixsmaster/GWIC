/*
 * CGWIC_Gizmo.h
 *  Created on: Dec 30, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_GIZMO_H_
#define CGWIC_GIZMO_H_

#include <irrlicht.h>
#include <GWICDefines.h>
#include <GWICTypes.h>
#include "CPoint2D.h"

namespace gwic {

class CGWIC_Gizmo {
public:
	CGWIC_Gizmo(irr::IrrlichtDevice* dev);
	CGWIC_Gizmo(irr::IrrlichtDevice* dev, irr::core::vector3df pos);
	virtual ~CGWIC_Gizmo();
	void ProcessEvent(const irr::SEvent& event);
	irr::core::vector3df GetAbsolutePosition() { return this->abspos; }
	CPoint2D GetCurrentCell() { return this->cell; }
	irr::core::vector3df GetCellRelativePosMetric();
	irr::core::vector3df GetDifference() { return this->lastdiff; }
	irr::core::vector3df GetDifferenceMetric() { return (this->lastdiff / GWIC_IRRUNITS_PER_METER); }
	void SetCell(const CPoint2D nwcell) { this->cell = nwcell; }
	void SetCellRelativePosMetric(irr::core::vector3df vec);
	void SetAbsolutePosition(const irr::core::vector3df vec);
protected:
	CPoint2D cell;
	irr::core::vector3df abspos;
	irr::core::vector3df lastdiff;
	irr::IrrlichtDevice* irDevice;
	std::vector<irr::scene::ISceneNode*> handles;
	void UpdateCell();
	void CreateArrows();
};

} /* namespace gwic */
#endif /* CGWIC_GIZMO_H_ */
