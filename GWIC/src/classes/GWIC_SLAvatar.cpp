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

GWIC_SLAvatar::GWIC_SLAvatar(irr::io::path xmlfile, irr::IrrlichtDevice* irrdev)
{
	irrDevice = irrdev;
	body = NULL;
	std::cout << "SLAvatar c'tor" << std::endl;
	// just a test
	IMesh* mesh = irrDevice->getSceneManager()->getMesh(GWIC_SLRELATIVE_DIR+path("avatar_upper_body.llm"));
	if (!mesh) {
		std::cerr << "Unable to load upper body" << std::endl;
		return;
	}
	body = irrDevice->getSceneManager()->addMeshSceneNode(mesh,NULL,GWIC_PICKABLE_MASK | GWIC_ACTOR_MASK);
	if (body) std::cout << "Mesh loading done" << std::endl;
}

GWIC_SLAvatar::~GWIC_SLAvatar()
{
	std::cout << "SLAvatar d'tor" << std::endl;
}


} /* namespace gwic */
