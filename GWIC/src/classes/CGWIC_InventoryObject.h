/*
 * CGWIC_InventoryObject.h
 *  Created on: Dec 22, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_INVENTORYOBJECT_H_
#define CGWIC_INVENTORYOBJECT_H_

#include <GWICTypes.h>
#include <irrlicht.h>
#include "CGWIC_GameObject.h"
#include "CGWIC_BodyPart.h"

namespace gwic {

enum GWIC_INVENTORY_TYPE {
	GWIC_ITYPE_DUMMY,
	GWIC_ITYPE_FOLDER,
	GWIC_ITYPE_TEXTNOTE,
	GWIC_ITYPE_SCRIPT,
	GWIC_ITYPE_GAMEOBJECT,
	GWIC_ITYPE_BODYPART,
	GWIC_ITYPE_TEXTURE,
	GWIC_ITYPE_MATERIAL,
	GWIC_ITYPE_TELEPORT
};

class CGWIC_InventoryObject {
public:
	CGWIC_InventoryObject();
	virtual ~CGWIC_InventoryObject();
	GWIC_INVENTORY_TYPE GetType();
	irr::core::stringw GetName();
protected:
	GWIC_INVENTORY_TYPE otype;
	irr::core::stringw oname;
};

} /* namespace gwic */
#endif /* CGWIC_INVENTORYOBJECT_H_ */
