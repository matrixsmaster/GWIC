/*
 * CGWICWindowUI.cpp
 *  Created on: Dec 29, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWICWindowUI.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_UIWindow::~CGWIC_UIWindow()
{
	// TODO Auto-generated destructor stub
}

bool CGWIC_UIWindow::LoadFromFile(const path filename)
{
	IXMLReader* xml = irDevice->getFileSystem()->createXMLReader(filename);
	if (!xml) {
		std::cerr << "Window description XML file not found!" << std::endl;
		return false;
	}
	std::cout << "Loading UI XML: " << filename.c_str() << std::endl;
	xml->drop();
	return true;
}

void CGWIC_UIWindow::PumpMessage(const irr::SEvent& event)
{
	//
}

void CGWIC_UIWindow::Update()
{
	//
}


} /* namespace gwic */
