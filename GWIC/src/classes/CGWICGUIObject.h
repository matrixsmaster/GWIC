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
	virtual void PumpMessage(const irr::SEvent& event) = 0;
	virtual bool LoadFromFile(const irr::io::path filename) { return false; }
	void SetName(irr::core::stringc nwname) { this->myname = nwname; }
	irr::core::stringc GetName() { return this->myname; }
	virtual void SetPos(CPoint2D nwpos);
	virtual CPoint2D GetPos() { return this->basepoint; }
	virtual void SetVisible(bool setup);
	bool GetVisible() { return this->visible; }
	virtual void Update() = 0;
	virtual irr::core::stringw GetNextCommand();
	virtual void FlushBuffers();
protected:
	irr::core::stringc myname;
	irr::IrrlichtDevice* irDevice;
	irr::gui::IGUIEnvironment* GUI;
	CPoint2D basepoint;
	bool visible;
	std::vector<irr::core::stringw> cmdfifo;
	std::vector<irr::gui::IGUIElement*> elems;
};

} /* namespace gwic */
#endif /* CGWICGUIOBJECT_H_ */
