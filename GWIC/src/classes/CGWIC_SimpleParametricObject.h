/*
 * CGWIC_SimpleParametricObject.h
 *
 *  Created on: Mar 21, 2013
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_SIMPLEPARAMETRICOBJECT_H_
#define CGWIC_SIMPLEPARAMETRICOBJECT_H_

#include "CGWIC_GameObject.h"

namespace gwic {

enum GWIC_SimpleParametricStyle {
	GWIC_ParametricCube,
	GWIC_ParametricCylinder,
	GWIC_ParametricSphere
};

struct SGWIC_SimpleParametric {
	GWIC_SimpleParametricStyle style;
	float a,b,c;
};

class CGWIC_SimpleParametricObject: public gwic::CGWIC_GameObject {
public:
	CGWIC_SimpleParametricObject();
	virtual ~CGWIC_SimpleParametricObject();
};

} /* namespace gwic */
#endif /* CGWIC_SIMPLEPARAMETRICOBJECT_H_ */
