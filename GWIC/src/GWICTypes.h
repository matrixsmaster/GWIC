/*
 * GWICTypes.h
 *  Created on: 06.12.2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012-2013
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include <CPoint2D.h>
#include <CIrrStrParser.h>

#ifndef GWICTYPES_H_
#define GWICTYPES_H_

#include <irrlicht.h>
#include <iostream>
#include <GWICDefines.h>

typedef unsigned char uchar;

namespace irr {

typedef core::aabbox3d<f32> boundbox;

}

namespace gwic {

/*
struct SPoint2D {
	int x;
	int y;
};
typedef struct SPoint2D TPoint2D;
*/

enum ObjMaterialType {
	MATERIAL_SIMPLE,
	MATERIAL_TEXTURED, //??
	MATERIAL_IRRLICHT
}; // I'm not sure that's needed at all =/

struct ObjMaterial {
	ObjMaterialType type;
	irr::video::SMaterial irrmat;
};

struct HardCullingProperties {
	float ObjectCullMeters;
	float ActorsCullMeters;
	bool DistantLand;
};

struct WorldProperties {
	int gWidth;
	int gHeight;
	int bits;
	bool fullscreen;
	bool stencil;
	bool vsync;
	irr::video::E_DRIVER_TYPE videoDriver;
	irr::io::path vfsRoot;
	irr::io::E_FILE_ARCHIVE_TYPE vfsType;
	int wrldSizeX;
	int wrldSizeY;
	int startX;
	int startY;
	float minLevel;
	float waterLevel;
	bool debugdraw;
	float Gforce;
	float viewDistance;
	irr::s32 maxLOD;
	HardCullingProperties hardcull;
};

struct GWIC_Message {
	irr::core::stringw sender;
	irr::core::stringw recepient;
	irr::s32 index;
	irr::s32 lifetime;
	irr::core::stringw body;
};

enum GWICVM_VMState {
	GWICVM_EMPTY,	// no code loaded, empty VM couldn't be running
	GWICVM_IDLE,	// some code loaded and compiled successfully, VM can run
	GWICVM_ERROR,	// code loaded with errors
	GWICVM_RUN,		// VM running
	GWICVM_PAUSE	// VM paused
};


}

#endif /* GWICTYPES_H_ */
