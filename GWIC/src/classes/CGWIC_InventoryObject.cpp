/*
 * CGWIC_InventoryObject.cpp
 *  Created on: Dec 22, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWIC_InventoryObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_InventoryObject::CGWIC_InventoryObject(GWIC_INVENTORY_TYPE newtype, irr::IrrlichtDevice* irrdev)
{
	irrDevice = irrdev;
	otype = newtype;
	oname = L"Untitled";
	//init all pointers
	gobject = NULL;
	bpart = NULL;
	image = NULL;
	omatptr = NULL;
}

CGWIC_InventoryObject::~CGWIC_InventoryObject()
{
	mtext.clear();
	if (gobject) delete gobject;
	if (bpart) delete bpart;
	if (image) image->drop();
	if (omatptr) delete omatptr;
}

void CGWIC_InventoryObject::SetName(irr::core::stringw newname)
{
	if (!newname.empty()) oname = newname;
}

irr::core::stringc CGWIC_InventoryObject::TypeToString(const GWIC_INVENTORY_TYPE typ)
{
	int n = static_cast<int> (typ);
	int m = static_cast<int> (GWIC_ITYPE_DUMMY);
	stringc res("dummy");
	if ((n<0) || (n>m)) return res;
	res = GWIC_InventoryTypesStrings[n];
	return res;
}

GWIC_INVENTORY_TYPE CGWIC_InventoryObject::StringToType(const irr::core::stringc str)
{
	int i = 0;
	GWIC_INVENTORY_TYPE res;
	do res = static_cast<GWIC_INVENTORY_TYPE> (i);
	while ((res != GWIC_ITYPE_DUMMY) &&
			(!str.equals_ignore_case(GWIC_InventoryTypesStrings[i])));
	//dummy type means dummy or unknown ;)
	return res;
}

bool CGWIC_InventoryObject::SerializeToFile(irr::io::path filename)
{
	//TODO
	return false;
}

void CGWIC_InventoryObject::Assign(CGWIC_InventoryObject* src)
{
	//TODO
}

bool CGWIC_InventoryObject::LoadFromFile(irr::io::path filename)
{
	IXMLReader* xml = irrDevice->getFileSystem()->createXMLReader(filename);
	if (!xml) {
		std::cerr << "Unable to open inventory item XML: " << filename.c_str() << std::endl;
		return false;
	}
	std::cout << "Reading inventory data from " << filename.c_str() << std::endl;
	const stringw tag_type(L"type");
	const stringw tag_name(L"name");
	const stringw tag_childs(L"childs");
	const stringw tag_body(L"body");
	while (xml->read()) {
		if (tag_type.equals_ignore_case(xml->getNodeName())) {
			otype = this->StringToType(xml->getAttributeValueSafe(L"str"));
		} else if (tag_name.equals_ignore_case(xml->getNodeName())) {
			oname = xml->getAttributeValueSafe(L"str");
		} else if (tag_childs.equals_ignore_case(xml->getNodeName())) {
			//
		} else if (tag_body.equals_ignore_case(xml->getNodeName())) {
			//
		}
	}
	xml->drop();
	return true;
}


} /* namespace gwic */
