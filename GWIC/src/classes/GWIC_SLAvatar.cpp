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

GWIC_SLAvatar::GWIC_SLAvatar(irr::io::path xmlfile, irr::scene::ISceneManager* mgr) :
	ISceneNode(NULL,mgr,GWIC_ACTOR_MASK){
	scManager = mgr;
	Box.reset(0,0,0);
}

GWIC_SLAvatar::~GWIC_SLAvatar()
{
	// TODO Auto-generated destructor stub
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
