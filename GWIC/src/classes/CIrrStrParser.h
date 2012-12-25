/*
 * CIrrStrParser.h
 *  Created on: 25.12.2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CIRRSTRPARSER_H_
#define CIRRSTRPARSER_H_

#include <stdio.h>
#include <vector>
#include <CPoint2D.h>
#include <irrlicht.h>

namespace gwic {

typedef std::vector<irr::core::stringw> irrstrwvec;

class CIrrStrParser {
public:
	CIrrStrParser();
	CIrrStrParser(irr::core::stringw str);
	virtual ~CIrrStrParser();
	irr::core::vector3df ToVector3f();
	irr::core::position2di ToPosition2i();
	CPoint2D ToPoint2D();
	float ToFloat();
	irr::s32 ToS32();
	irr::core::stringw NextLex(irr::core::stringw delim, bool erase);
	irrstrwvec ParseToList(irr::core::stringw delim);
	irr::core::stringw GetBuff() { return this->buffer; }
	CIrrStrParser & operator = (const CIrrStrParser & other) { buffer = other.buffer; return *this; }
	CIrrStrParser & operator = (const irr::core::stringw & obuf) { buffer = obuf; return *this; }
	CIrrStrParser operator + (const CIrrStrParser & addc) const { return CIrrStrParser(buffer + addc.buffer); }
	CIrrStrParser & operator += (const CIrrStrParser & addc) { buffer += addc.buffer; return *this; }
	CIrrStrParser & operator += (const irr::core::vector3df & addv);
	CIrrStrParser & operator += (const irr::core::position2di & addp);
	CIrrStrParser & operator += (const CPoint2D & addpp);
protected:
	irr::core::stringw buffer;
};

} /* namespace gwic */
#endif /* CIRRSTRPARSER_H_ */
