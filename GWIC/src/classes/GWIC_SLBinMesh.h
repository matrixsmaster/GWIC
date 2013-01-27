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

namespace gwic {

static const char LLBinMesh_Header[24] = "Linden Binary Mesh 1.0";

class GWIC_SLBinMesh: public irr::scene::SMesh {
public:
	GWIC_SLBinMesh(irr::io::path filename);
	virtual ~GWIC_SLBinMesh();
};

} /* namespace gwic */
#endif /* GWIC_SLBINMESH_H_ */
