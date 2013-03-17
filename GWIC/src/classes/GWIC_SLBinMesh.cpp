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
	std::cout << "GWIC_SLBinMesh created" << std::endl;
}

GWIC_SLBinMesh::~GWIC_SLBinMesh()
{
	std::cout << "GWIC_SLBinMesh destroyed" << std::endl;
}

bool GWIC_SLBinMesh::isALoadableFileExtension(const irr::io::path& filename) const
{
	return hasFileExtension(filename,"llm");
}

irr::scene::IAnimatedMesh* GWIC_SLBinMesh::createMesh(irr::io::IReadFile* file)
{
	std::cout << "Loading SLBinMesh" << std::endl;
	return 0;
	/*
	if (!loadbin(filename))
		std::cerr << "Can't load SLBinMesh from " << filename.c_str() << std::endl;*/
}

} /* namespace gwic */
