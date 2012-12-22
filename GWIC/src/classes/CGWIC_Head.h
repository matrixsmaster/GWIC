/*
 * CGWIC_Head.h
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWIC_HEAD_H_
#define CGWIC_HEAD_H_

#include <iostream>
#include <irrlicht.h>
#include <irrbullet.h>
#include <GWICTypes.h>
#include "CGWIC_BodyPart.h"

namespace gwic {

class CGWIC_Head: public gwic::CGWIC_BodyPart {
public:
	CGWIC_Head(irr::io::path modelfile, irr::scene::ISceneNode* parentn, irr::IrrlichtDevice* dev, irrBulletWorld* phy) :
		CGWIC_BodyPart(modelfile,parentn,dev,phy) {
		std::cout << "Head {} creation" << std::endl;
		name = "HEAD";
	}
	virtual ~CGWIC_Head();
};

}

#endif /* CGWIC_HEAD_H_ */
