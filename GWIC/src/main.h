/*
 * main.h
 *  Created on: Dec 3, 2012
 *
 *  GWIC (C) Dmitry Soloviov, 2012-2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <GWICTypes.h>
#include <GWICDefines.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <irrlicht.h>
#include "classes/CGWIC_World.h"
#include <OALWrapper/OAL_Types.h>
#include <OALWrapper/OAL_Funcs.h>

#define GWIC_KEYS_ARR_SIZE 2

// GLOBAL Variables for this main module
gwic::CGWIC_World* ourworld;
gwic::WorldProperties wrldSettings;
extern cOAL_Device* gpDevice;

enum GWIC_ARGKEYTYPE {
	GWIC_AKEY_VFSROOT = 0,
	GWIC_AKEY_VFSTYPE,
	GWIC_AKEY_UNKNOWN
};

static const char * GWIC_KEYS_ARRAY[GWIC_KEYS_ARR_SIZE] = {
		"-root",
		"-atype"
};

#endif /* MAIN_H_ */
