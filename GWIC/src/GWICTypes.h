/*
 * GWICTypes.h
 *  Created on: 06.12.2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012
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

namespace irr {

typedef core::aabbox3d<float> boundbox;

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
	MATERIAL_TEXTURED,
	MATERIAL_IRRLICHT
};

struct ObjMaterial {
	ObjMaterialType type;
	bool SphericalMapped;
	irr::io::path texture;
};

struct WorldProperties {
	int gWidth;
	int gHeight;
	int bits;
	bool fullscreen;
	bool stencil;
	bool vsync;
	irr::video::E_DRIVER_TYPE videoDriver;
	irr::core::stringw vfsRoot;
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
};

}

#endif /* GWICTYPES_H_ */
