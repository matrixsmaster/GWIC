/*
 * GWIC_SLAvatar.cpp
 *
 *  Created on: Jan 25, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "GWIC_SLAvatar.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;

namespace gwic {

GWIC_SLAvatar::GWIC_SLAvatar(irr::io::path xmlfile, irr::IrrlichtDevice* irrdev) :
	ISceneNode(NULL,irrdev->getSceneManager(),GWIC_ACTOR_MASK){
	irrDevice = irrdev;
	Box.reset(0,0,0);
	body = NULL;
}

GWIC_SLAvatar::~GWIC_SLAvatar()
{
	//
}

const irr::boundbox& GWIC_SLAvatar::getBoundingBox() const
{
	return Box;
}

void GWIC_SLAvatar::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);
	ISceneNode::OnRegisterSceneNode();
}

void GWIC_SLAvatar::render()
{
	IVideoDriver* driver = SceneManager->getVideoDriver();
}


} /* namespace gwic */
