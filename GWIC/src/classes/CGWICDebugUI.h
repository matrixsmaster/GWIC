/*
 * CGWICDebugUI.h
 *  Created on: Dec 15, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWICDEBUGUI_H_
#define CGWICDEBUGUI_H_

#include <iostream>
#include <irrlicht.h>
#include <irrbullet.h>
#include <GWICTypes.h>
#include "CIrrStrParser.h"
#include "CGWICGUIObject.h"

/*
 * This is hardcoded debug interface
 * even if nothing specified in program's args and no files found
 * on VFS or VFS itself is missing, this would work
 */

#define GWICDEBUG_LOGWIDTH 450
#define GWICDEBUG_LOGHEIGHT 90
#define GWICDEBUG_BUFSIZE 270

namespace gwic {

class CGWIC_DebugUI: public gwic::CGWIC_GUIObject {
public:
	CGWIC_DebugUI(irr::IrrlichtDevice* dev) :
		CGWIC_GUIObject(dev) {
		mytype = GWIC_UIOBJ_DEBUGUI;
		CreateHardcodedUI();
//		hist_it = histbuf.begin();
	}
	virtual ~CGWIC_DebugUI() { }
	void LogText(irr::core::stringw text);
	void SetVisible(bool setup);
	void HideTotally();
	void PumpMessage(const irr::SEvent& event);
	void Update() { }
	void UpdateFPS(int fps);
	void FlushBuffers();
	bool PutString(const irr::core::stringw str);
	irr::s32 GetRootID() { return -1; }
	irr::gui::IGUIElement* GetRootPtr() { return this->loglabel; }
private:
	irr::gui::IGUIStaticText* loglabel;
	irr::gui::IGUIStaticText* fpslabel;
	irr::core::stringw buffer;
	irr::gui::IGUIEditBox* input;
	irr::gui::IGUIButton* history;
	irrstrwvec histbuf;
//	irrstrwvec::iterator hist_it;
	void CreateHardcodedUI();
};

} /* namespace gwic */
#endif /* CGWICDEBUGUI_H_ */
