/*
 * GWICDefines.h
 *  Created on: 12.12.2012
 *
 *  GWIC (C) Dmitry Soloviov, 2012-2013
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef GWICDEFINES_H_
#define GWICDEFINES_H_

//#define GWIC_USES_OPENAL 1
#define GWIC_FILESYSTEMROOT irr::io::path("/tmp/GWIC/VFS")
#define GWIC_ROOTFS_TYPE irr::io::EFAT_FOLDER
#define GWIC_TEXTURES_DIR irr::io::path("textures/")
#define GWIC_ACTORS_DIR irr::io::path("actors/")
#define GWIC_BPARTS_DIR irr::io::path("bodyparts/")
#define GWIC_OBJECTS_DIR irr::io::path("gameobjects/")
#define GWIC_STATMESHES_DIR irr::io::path("static_meshes/")
#define GWIC_TERRAIN_DIR irr::io::path("terrain/")
#define GWIC_OGGSTREAM_DIR irr::io::path("music/")
#define GWIC_SOUNDS_DIR irr::io::path("sound_fx/")
#define GWIC_UI_DIR irr::io::path("ui/")
#define GWIC_FONTS_DIR irr::io::path("fonts/")

#define GWIC_MAINWINDOW_CAPTION L"GWIC ver.0.0.2.66"

#define GWIC_VOID_COLOR SColor(255,100,100,100)

#define GWIC_IRRUNITS_PER_METER 20.f
// don't change next V line!
#define GWIC_METERS_PER_CELL 240.f
#define GWIC_PHYS_TIMESCALE 0.001f
#define GWIC_PHYS_SUBSTEPS 90

#define Random_FLOAT(G) (float)rand()/((float)RAND_MAX/(float)(G));

#define GWIC_PICKABLE_MASK (1<<30)
#define GWIC_GIZMO_MASK (1<<29)

//u32 tesselationCylinder=4,u32 tesselationCone=8,
//f32 height=1.f, f32 cylinderHeight=0.6f,
//f32 widthCylinder=0.05f, f32 widthCone=0.3f
#define GWIC_GIZMO_ARROW_PARAMS 4, 8, 1.f, 0.6f, 0.05f, 0.15f
#define GWIC_GIZMO_X_COLOR SColor(255,255,0,0)
#define GWIC_GIZMO_Y_COLOR SColor(255,0,255,0)
#define GWIC_GIZMO_Z_COLOR SColor(255,0,0,255)
#define GWIC_GIZMO_SIZE_DIVIDER 3.f

#define GWIC_GUI_DEBUG_EDITBOX 11
#define GWIC_GUI_DEBUG_HDEPTH 12
#define GWIC_GUI_DEBUG_LAST 100
#define GWIC_GUI_WINDOW_MIN 100
#define GWIC_GUI_WINDOW_LOWBORDER 8
#define GWIC_GUI_LOADSCR_BAR_WD 64
#define GWIC_GUI_LOADSCR_BAR_BS 132
#define GWIC_GUI_LOADSCR_BAR_HG 32
#define GWIC_GUI_LOADSCR_BAR_BACKCOL SColor(255,32,0,32)
#define GWIC_GUI_LOADSCR_BAR_FORECOL SColor(255,230,0,0)
#define GWIC_GUI_LOADSCR_BAR_BORDCOL SColor(255,0,0,255)
#define GWIC_GUI_LOADSCR_BACKGROUND SColor(255,32,0,32)

#endif /* GWICDEFINES_H_ */
