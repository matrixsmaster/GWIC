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
#include "CIrrStrParser.h"

namespace gwic {

enum GWIC_INVENTORY_TYPE {
	GWIC_ITYPE_ROOT = 0,
	GWIC_ITYPE_FOLDER,
	GWIC_ITYPE_TEXT,
	GWIC_ITYPE_SCRIPT,
	GWIC_ITYPE_GAMEOBJECT,
	GWIC_ITYPE_BODYPART,
	GWIC_ITYPE_TEXTURE,
	GWIC_ITYPE_PHOTO,
	GWIC_ITYPE_MATERIAL,
	GWIC_ITYPE_TELEPORT,
	GWIC_ITYPE_DUMMY
};

static const irr::core::stringc GWIC_InventoryTypesStrings[] = {
		irr::core::stringc("ROOT"),
		irr::core::stringc("Folder"),
		irr::core::stringc("Text"),
		irr::core::stringc("Script"),
		irr::core::stringc("GameObject"),
		irr::core::stringc("BodyPart"),
		irr::core::stringc("Texture"),
		irr::core::stringc("Image"),
		irr::core::stringc("Material"),
		irr::core::stringc("Teleport"),
		irr::core::stringc("dummy")
};

class CGWIC_InventoryObject {
public:
	CGWIC_InventoryObject(GWIC_INVENTORY_TYPE newtype, irr::IrrlichtDevice* irrdev);
	virtual ~CGWIC_InventoryObject();
	GWIC_INVENTORY_TYPE GetType() { return this->otype; }
	irr::core::stringw GetName() { return this->oname; }
	void SetName(irr::core::stringw newname);
	irr::core::stringc GetTypeAsString() { return TypeToString(this->otype); }
	irr::core::stringc TypeToString(const GWIC_INVENTORY_TYPE typ);
	GWIC_INVENTORY_TYPE StringToType(const irr::core::stringc str);
	virtual bool SerializeToFile(irr::io::path filename);
	virtual void Assign(CGWIC_InventoryObject* src);
	virtual bool LoadFromFile(irr::io::path filename);
protected:
	irr::IrrlichtDevice* irrDevice;
	GWIC_INVENTORY_TYPE otype;
	irr::core::stringw oname;
	std::vector<CGWIC_InventoryObject*> childs;
private:
	irrstrwvec mtext;
	CGWIC_GameObject* gobject;
	CGWIC_BodyPart* bpart;
	irr::video::IImage* image;
	ObjMaterial* omatptr;
};

} /* namespace gwic */
#endif /* CGWIC_INVENTORYOBJECT_H_ */
