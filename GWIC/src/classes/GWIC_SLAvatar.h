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
#include "GWIC_SLBinMesh.h"

namespace gwic {

class GWIC_SLAvatar : public irr::scene::ISceneNode {
public:
	GWIC_SLAvatar(irr::io::path xmlfile, irr::IrrlichtDevice* irrdev);
	virtual ~GWIC_SLAvatar();
	const irr::boundbox& getBoundingBox() const;
	virtual void OnRegisterSceneNode();
	virtual void render();
private:
	irr::IrrlichtDevice* irrDevice;
	irr::boundbox Box;
	GWIC_SLBinMesh* body;
	std::vector<CGWIC_GameObject*> attachments;
};

} /* namespace gwic */
#endif /* GWIC_SLAVATAR_H_ */
