/*
 * GWIC_SLBinMesh.h
 *
 *  Created on: Jan 27, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef GWIC_SLBINMESH_H_
#define GWIC_SLBINMESH_H_

#include <irrlicht.h>
#include <iostream>

using namespace irr;

namespace gwic {

static const char LLBinMesh_Header[24] = "Linden Binary Mesh 1.0";

class GWIC_SLBinMesh: public scene::IMeshLoader {
public:
	GWIC_SLBinMesh(scene::ISceneManager* smgr, io::IFileSystem* fs);
	virtual ~GWIC_SLBinMesh();
	virtual bool isALoadableFileExtension(const io::path& filename) const;
	virtual scene::IAnimatedMesh* createMesh(io::IReadFile* file);
protected:
	//
private:
	//FIXME: this variables copied from C3DSMesh...; discard useless
	scene::ISceneManager* SceneManager;
	io::IFileSystem* FileSystem;
	f32* Vertices;
	u16* Indices;
	u32* SmoothingGroups;
	core::array<u16> TempIndices;
	f32* TCoords;
	u16 CountVertices;
	u16 CountFaces;
	u16 CountTCoords;
	core::array<core::stringc> MeshBufferNames;
	core::matrix4 TransformationMatrix;
	scene::SMesh* Mesh;
};

} /* namespace gwic */
#endif /* GWIC_SLBINMESH_H_ */
