/*
 * CGWIC_LoadingScreen.h
 *
 *  Created on: Jan 19, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_LOADINGSCREEN_H_
#define CGWIC_LOADINGSCREEN_H_

#include "GWICDefines.h"
#include "CProgressBar.h"

namespace irr {
namespace gui {
class CProgressBar;
}
}

namespace gwic {

class CGWIC_LoadingScreen {
public:
	CGWIC_LoadingScreen(irr::IrrlichtDevice* device);
	virtual ~CGWIC_LoadingScreen();
	void StartProcess(const irr::s32 ticksTotal, const irr::s32 progressDelta);
	bool ProcessTick();
//	void FinishProcess();
	void SetProgress(irr::s32 percent);
	irr::s32 GetProgress() { return this->progress; }
	bool Update();
private:
	irr::IrrlichtDevice* irDevice;
	irr::video::IVideoDriver* irDriver;
	irr::gui::IGUIEnvironment* GUI;
	irr::gui::CProgressBar* pBar;
	irr::s32 progress;
	float progPerTick;
	float fltProgress;
};

} /* namespace gwic */
#endif /* CGWIC_LOADINGSCREEN_H_ */
