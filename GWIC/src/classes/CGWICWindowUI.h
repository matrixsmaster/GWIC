/*
 * CGWICWindowUI.h
 *  Created on: Dec 29, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWICWINDOWUI_H_
#define CGWICWINDOWUI_H_

#include "CGWICGUIObject.h"

namespace gwic {

class CGWIC_UIWindow: public gwic::CGWIC_GUIObject {
public:
	CGWIC_UIWindow(irr::IrrlichtDevice* dev) :
		CGWIC_GUIObject(dev) {
		window = GUI->addWindow(irr::core::rect<irr::s32>(0,0,100,100),false,L"New window");
		if (!window) return;
		elems.push_back(window);
	}
	virtual ~CGWIC_UIWindow();
	bool LoadFromFile(const irr::io::path filename);
	void PumpMessage(const irr::SEvent& event);
	void Update();
	void PutString(const irr::core::stringw str);
	void SetPos(CPoint2D nwpos);
	irr::s32 GetRootID() { return this->window->getID(); }
private:
	irr::gui::IGUIWindow* window;
	std::vector<GWICActionPointer> acts;
	void ProcessAction(uint action, uint type);
};

} /* namespace gwic */
#endif /* CGWICWINDOWUI_H_ */
