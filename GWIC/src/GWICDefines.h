/*
 * GWICDefines.h
 *
 *  Created on: 12.12.2012
 *      Author: Soloviov Dmitry
 */

#ifndef GWICDEFINES_H_
#define GWICDEFINES_H_

//#define GWIC_USES_OPENAL 1
#define GWIC_FILESYSTEMROOT "/home/lisa/workspace/GWIC/VFS"
#define GWIC_ROOTFS_TYPE irr::io::EFAT_FOLDER
#define GWIC_MESHES_DIR "/meshes/"
#define GWIC_TEXTURES_DIR "/textures/"
#define GWIC_ACTORS_DIR "/actors/"
#define GWIC_OBJECTS_DIR "/game_objects/"
#define GWIC_OGGSTREAM_DIR "/music/"
#define GWIC_SOUNDS_DIR "/sound_fx/"

#define GWIC_MAINWINDOW_CAPTION L"GWIC ver.0.0.1"

#define GWIC_IRRUNITS_PER_METER 20
// don't change next line!
#define GWIC_METERS_PER_CELL 240

#define Random_FLOAT(G) (float)rand()/((float)RAND_MAX/(float)G);

#define GWIC_PICKABLE_MASK (1<<30)

#define GWIC_GUI_DEBUG_EDITBOX 11

#endif /* GWICDEFINES_H_ */
