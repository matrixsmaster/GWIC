/*
 * CGWICGUIObject.h
 *  Created on: Dec 15, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWICGUIOBJECT_H_
#define CGWICGUIOBJECT_H_

#include <iostream>
#include <stdio.h>
#include <irrlicht.h>
#include <irrbullet.h>
#include <GWICTypes.h>

namespace gwic {

class CGWIC_GUIObject {
public:
	CGWIC_GUIObject(irr::IrrlichtDevice* dev);
	virtual ~CGWIC_GUIObject();
	virtual void PumpMessage(const irr::SEvent& event);
	virtual bool LoadFromFile(const irr::core::stringw filename);
	void SetName(irr::core::stringc nwname);
	irr::core::stringc GetName();
	void SetPos(CPoint2D nwpos);
	CPoint2D GetPos();
	virtual void SetVisible(bool setup);
	bool GetVisible();
	virtual void Update();
	virtual void Process();
protected:
	irr::core::stringc myname;
	irr::IrrlichtDevice* irDevice;
	irr::gui::IGUIEnvironment* GUI;
	CPoint2D basepoint;
	bool visible;
};

} /* namespace gwic */
#endif /* CGWICGUIOBJECT_H_ */
