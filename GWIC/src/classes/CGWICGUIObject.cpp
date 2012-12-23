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

CGWIC_GUIObject::~CGWIC_GUIObject()
{
	// TODO Auto-generated destructor stub
}

void CGWIC_GUIObject::PumpMessage(const irr::SEvent& event)
{
	//
}

bool CGWIC_GUIObject::LoadFromFile(const irr::core::stringw filename)
{
	IXMLReader* xml = irDevice->getFileSystem()->createXMLReader(filename);
	std::cout << "Loading UI XML: " << filename.c_str() << std::endl;
	xml->drop();
	return true;
}

void CGWIC_GUIObject::SetName(irr::core::stringc nwname)
{
	myname = nwname;
}

irr::core::stringc CGWIC_GUIObject::GetName()
{
	return this->myname;
}

void CGWIC_GUIObject::SetPos(CPoint2D nwpos)
{
	basepoint = nwpos;
}

CPoint2D CGWIC_GUIObject::GetPos()
{
	return this->basepoint;
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

bool CGWIC_GUIObject::GetVisible()
{
	return this->visible;
}

void CGWIC_GUIObject::Update()
{
	//
}

void CGWIC_GUIObject::Process()
{
	//
}


} /* namespace gwic */
