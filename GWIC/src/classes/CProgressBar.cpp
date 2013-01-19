/*
 * CProgressBar.h
 *
 *  Created on: Jan 19, 2013
 *  Based on CProgressBar class by Zurzaza (Irrlicht off. forums)
 *  found here: http://irrlicht.sourceforge.net/forum/viewtopic.php?t=39356
 *
 *  Authors:
 *  	Zurzaza
 *  	matrixsmaster
 */

#include "CProgressBar.h"

namespace irr {

using namespace core;
using namespace video;

namespace gui {

CProgressBar::CProgressBar(IGUIEnvironment* guienv, IGUIElement* parent, const rect<s32>& rectangle, s32 id) :
	IGUIElement(EGUIET_ELEMENT,guienv,parent,id,rectangle)
{
	total = rectangle.LowerRightCorner.X - rectangle.UpperLeftCorner.X;
	gui = guienv;
	bar = rectangle;
	if (!parent)
		guienv->getRootGUIElement()->addChild(this);
	fillcolor.set(255,255,255,255);
	emptycolor.set(255,0,0,0);
	border = bar;
	this->setProgress(0);
}

void CProgressBar::setColors(SColor progress, SColor filling)
{
	fillcolor = progress;
	emptycolor = filling;
}

void CProgressBar::addBorder(s32 size, SColor color)
{
	bordercolor = color;
	border = bar;
	border.UpperLeftCorner.X -= size;
	border.UpperLeftCorner.Y -= size;
	border.LowerRightCorner.X += size;
	border.LowerRightCorner.Y += size;
}

void CProgressBar::setProgress(u32 progress)
{
	if (progress > 100) progress = 0;
	u32 xpercentage;
	xpercentage = (progress * total)/100; //Reducing to the bar size
	tofill.UpperLeftCorner.set(bar.UpperLeftCorner.X,bar.UpperLeftCorner.Y);
	tofill.LowerRightCorner.set(bar.UpperLeftCorner.X+xpercentage,bar.LowerRightCorner.Y);
	empty.UpperLeftCorner.set(tofill.LowerRightCorner.X,tofill.UpperLeftCorner.Y);
	empty.LowerRightCorner.set(bar.LowerRightCorner.X,bar.LowerRightCorner.Y);
}

void CProgressBar::draw()
{
	if (this->IsVisible == false) return;
	irr::video::IVideoDriver* drv = gui->getVideoDriver();
	drv->draw2DRectangle(bordercolor,border);
	drv->draw2DRectangle(fillcolor,tofill);
	drv->draw2DRectangle(emptycolor,empty);
}

} /* namespace gui */
} /* namespace irr */
