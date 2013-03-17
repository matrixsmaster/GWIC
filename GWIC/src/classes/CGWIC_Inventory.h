/*
 * CGWIC_Inventory.h
 *  Created on: Dec 22, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_INVENTORY_H_
#define CGWIC_INVENTORY_H_

#include <GWICTypes.h>
#include <irrlicht.h>
#include "CIrrStrParser.h"
#include "CGWIC_InventoryObject.h"

namespace gwic {

class CGWIC_Inventory {
public:
	CGWIC_Inventory(irr::IrrlichtDevice* irrdev);
	virtual ~CGWIC_Inventory();
	irrstrwvec GetItemsList();
	bool LoadFromFile(irr::io::path filename);
protected:
	irr::IrrlichtDevice* irrDevice;
	std::vector<CGWIC_InventoryObject*> items;
};

} /* namespace gwic */
#endif /* CGWIC_INVENTORY_H_ */
