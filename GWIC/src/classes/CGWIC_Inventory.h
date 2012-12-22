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
#include "CGWIC_InventoryObject.h"

namespace gwic {

class CGWIC_Inventory {
public:
	CGWIC_Inventory();
	virtual ~CGWIC_Inventory();
	std::vector<irr::core::stringw> GetItemsList();
protected:
	std::vector<CGWIC_InventoryObject*> items;
};

} /* namespace gwic */
#endif /* CGWIC_INVENTORY_H_ */
