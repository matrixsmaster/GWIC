/*
 * CGWICGUIObject.cpp
 *  Created on: Dec 15, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWICGUIObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_GUIObject::CGWIC_GUIObject(irr::IrrlichtDevice* dev)
{
	irDevice = dev;
	GUI = dev->getGUIEnvironment();
	basepoint = CPoint2D(0);
	myname = L"GUIObject";
	visible = true;
	std::cout << "Created new GUI object" << std::endl;
}

void CGWIC_GUIObject::SetPos(CPoint2D nwpos)
{
	CPoint2D delta = nwpos - basepoint;
//	rect<s32> rct;
	vector2d<signed int> lcorner;
	for (u32 i=0; i<elems.size(); i++) {
		lcorner = elems[i]->getRelativePosition().UpperLeftCorner;
		elems[i]->setRelativePosition(position2di(lcorner.X+delta.X,lcorner.Y+delta.Y));
	}
	basepoint = nwpos;
}

void CGWIC_GUIObject::SetVisible(bool setup)
{
	if (setup == visible) return;
	if (setup) {
		//show interface
	} else {
		//hide interface
	}
	visible = setup;
}

irr::core::stringw CGWIC_GUIObject::GetNextCommand()
{
	stringw cmd;
	if (cmdfifo.size()) {
		cmd = cmdfifo.back();
		cmdfifo.pop_back();
	}
	return cmd;
}

void CGWIC_GUIObject::FlushBuffers()
{
	cmdfifo.clear();
}

CGWIC_GUIObject::~CGWIC_GUIObject()
{
	std::cout << "Destroying GUI object " << myname.c_str() << std::endl;
	while (elems.size()) {
		if (elems.back()) elems.back()->remove();
		elems.pop_back();
	}
}


} /* namespace gwic */
