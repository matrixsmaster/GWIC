/*
 * CGWIC_LoadingScreen.cpp
 *
 *  Created on: Jan 19, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWIC_LoadingScreen.h"
#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_LoadingScreen::CGWIC_LoadingScreen(irr::IrrlichtDevice* device)
{
	irDevice = device;
	irDriver = device->getVideoDriver();
	GUI = device->getGUIEnvironment();
	rect<s32> vport = irDriver->getViewPort();
	rect<s32> pbar_rect(GWIC_GUI_LOADSCR_BAR_WD,0,0,0);
	pbar_rect.LowerRightCorner.X = vport.LowerRightCorner.X - GWIC_GUI_LOADSCR_BAR_WD;
	pbar_rect.UpperLeftCorner.Y = vport.LowerRightCorner.Y - GWIC_GUI_LOADSCR_BAR_BS;
	pbar_rect.LowerRightCorner.Y = pbar_rect.UpperLeftCorner.Y + GWIC_GUI_LOADSCR_BAR_HG;
	pBar = new CProgressBar(GUI,NULL,pbar_rect);
	if (pBar) {
		pBar->setProgress(0);
		pBar->addBorder(3);
	}
	progress = 0;
	progPerTick = 0.5f;
	fltProgress = 0;
}

CGWIC_LoadingScreen::~CGWIC_LoadingScreen()
{
	if (pBar) pBar->remove();
}

void CGWIC_LoadingScreen::StartProcess(const irr::s32 ticksTotal, const irr::s32 progressDelta)
{
	progPerTick = static_cast<float>(progressDelta) / static_cast<float>(ticksTotal);
	fltProgress = static_cast<float>(progress);
}

bool CGWIC_LoadingScreen::ProcessTick()
{
	fltProgress += progPerTick;
	progress = static_cast<s32> (floor(fltProgress));
	return (Update());
}

void CGWIC_LoadingScreen::SetProgress(irr::s32 percent)
{
	progress = percent;
	Update();
}

bool CGWIC_LoadingScreen::Update()
{
	while (progress > 100)
		progress = progress - 100;
	if (progress < 0) progress = 0;
	if (pBar) pBar->setProgress(static_cast<u32>(progress));
	if (!irDevice->run()) return false;
	irDriver->beginScene(true,true,GWIC_VOID_COLOR);
	GUI->drawAll();
	irDriver->endScene();
	return true;
}

} /* namespace gwic */
