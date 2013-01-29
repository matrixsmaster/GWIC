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
#include "CIrrStrParser.h"

namespace gwic {

enum GWIC_UIOBJ_TYPE {
	GWIC_UIOBJ_UNDEF,
	GWIC_UIOBJ_DEBUGUI,
	GWIC_UIOBJ_WNDUI
};

class CGWIC_GUIObject {
public:
	CGWIC_GUIObject(irr::IrrlichtDevice* dev);
	virtual ~CGWIC_GUIObject();
	virtual void PumpMessage(const irr::SEvent& event) = 0;
	virtual bool LoadFromFile(const irr::io::path filename) { return false; }
	void SetName(irr::core::stringc nwname) { this->myname = nwname; }
	irr::core::stringc GetName() { return this->myname; }
	virtual GWIC_UIOBJ_TYPE GetType() { return this->mytype; }
	virtual void SetPos(CPoint2D nwpos);
	virtual CPoint2D GetPos() { return this->basepoint; }
	virtual void SetVisible(bool setup);
	bool GetVisible() { return this->visible; }
	virtual void Update() = 0;
	virtual irr::core::stringw GetNextCommand();
	virtual void FlushBuffers();
	virtual bool PutString(const irr::core::stringw str) = 0;
	void SetNextID(irr::s32 nID) { currID = nID; }
	irr::s32 IterateID() { return (++currID); }
	virtual irr::s32 GetRootID() = 0;
	virtual irr::gui::IGUIElement* GetRootPtr() = 0;
protected:
	irr::s32 currID;
	irr::core::stringc myname;
	GWIC_UIOBJ_TYPE mytype;
	irr::IrrlichtDevice* irDevice;
	irr::gui::IGUIEnvironment* GUI;
	CPoint2D basepoint;
	bool visible;
	irrstrwvec cmdfifo;
	std::vector<irr::gui::IGUIElement*> elems;
};

struct GWICActionPointer {
	std::vector<irr::gui::EGUI_EVENT_TYPE> type;
	irr::s32 gid;
	irr::gui::IGUIElement* ptr;
	bool init_done;
	irr::core::stringw command;
	irr::core::stringw accept_filter;
};

} /* namespace gwic */
#endif /* CGWICGUIOBJECT_H_ */
