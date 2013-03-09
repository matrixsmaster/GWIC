/*
 * CGWIC_VM.cpp
 *  Created on: Dec 22, 2012
 *
 *	GWIC (c) 2012 Soloviov Dmitry
 *
 *	Virtual Machine class module
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWIC_VM.h"
#include "CGWIC_World.h"
#include "CGWIC_Cell.h"
#include "CGWIC_GameObject.h"
#include "CGWIC_Bot.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

struct CGWIC_VM::GWICVM_hData {
	CGWIC_World* world;
};

CGWIC_VM::CGWIC_VM(void *worldptr)
{
	hData = new GWICVM_hData();
	hData->world = reinterpret_cast<CGWIC_World*> (worldptr);
	luavm = NULL;
	curstate = GWICVM_EMPTY;
}

CGWIC_VM::~CGWIC_VM()
{
	if (luavm) lua_close(luavm);
	delete hData;
}

} /* namespace gwic */
