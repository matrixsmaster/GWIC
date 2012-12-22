/*
 * main.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Soloviov Dmitry
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

// GLOBAL Variables for this main module
gwic::CGWIC_World* ourworld;
gwic::WorldProperties wrldSettings;
extern cOAL_Device* gpDevice;


#endif /* MAIN_H_ */
