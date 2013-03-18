/*
 * GWIC_SLAvatar.h
 *
 *  Created on: Jan 25, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef GWIC_SLAVATAR_H_
#define GWIC_SLAVATAR_H_

#include <irrlicht.h>
#include <irrbullet.h>
#include <bulletworld.h>
#include <GWICDefines.h>
#include "GWICTypes.h"
#include "CGWIC_GameObject.h"

namespace gwic {

class GWIC_SLAvatar {
public:
	GWIC_SLAvatar(irr::io::path xmlfile, irr::IrrlichtDevice* irrdev);
	virtual ~GWIC_SLAvatar();
	irr::scene::ISceneNode* GetRoot() { return this->body; }
private:
	irr::IrrlichtDevice* irrDevice;
	irr::scene::ISceneNode* body;
	std::vector<CGWIC_GameObject*> attachments;
};

} /* namespace gwic */
#endif /* GWIC_SLAVATAR_H_ */
