/*
 * GWIC_SLBinMesh.cpp
 *
 *  Created on: Jan 27, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "GWIC_SLBinMesh.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

GWIC_SLBinMesh::GWIC_SLBinMesh(ISceneManager* smgr, IFileSystem* fs) :
		SceneManager(smgr), FileSystem(fs), Vertices(0), Indices(0), SmoothingGroups(0), TCoords(0),
		CountVertices(0), CountFaces(0), CountTCoords(0), Mesh(0)
{
	std::cout << "GWIC_SLBinMesh reader created" << std::endl;
}

GWIC_SLBinMesh::~GWIC_SLBinMesh()
{
	std::cout << "GWIC_SLBinMesh reader destroyed" << std::endl;
}

bool GWIC_SLBinMesh::isALoadableFileExtension(const irr::io::path& filename) const
{
	return hasFileExtension(filename,"llm");
}

irr::scene::ISkinnedMesh* GWIC_SLBinMesh::createMesh(irr::io::IReadFile* file)
{
	if (!file) return NULL;
	std::cout << "Loading SLBinMesh" << std::endl;
	LLBinMeshHeader head;
	file->read(&head,sizeof(LLBinMeshHeader));
#ifdef __BIG_ENDIAN__
	//FIXME: add rotation
#endif
	//FIXME: compare header strings
	//mMesh = new CSkinnedMesh();
	/*
	 * NOTE: to load complex skinned models with embedded deformations and morphing,
	 * we need to patch the Irrlicht itself, since there's no "public" interfaces
	 * or classes such a CSkinnedMesh, opened in library for outside use.
	 * So, patching the engine and adding the completely new class
	 * IDeformableSkinnedMesh is the way we need to follow ;)
	 * We may use CB3DMeshFileLoader as a reference for implement our own loader
	 * and http://wiki.secondlife.com/wiki/Avatar_Appearance
	 * as a reference for LL's models
	 */
	return 0;
}

} /* namespace gwic */
