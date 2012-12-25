/*
 * CIrrStrParser.cpp
 *
 *  Created on: 25.12.2012
 *      Author: master
 */

#include "CIrrStrParser.h"

using namespace irr;
using namespace io;
using namespace core;

namespace gwic {

CIrrStrParser::CIrrStrParser()
{
	buffer = "";
}

CIrrStrParser::CIrrStrParser(irr::core::stringw str)
{
	buffer = str;
}

irr::core::vector3df CIrrStrParser::ToVector3f()
{
	vector3df vec(0);
	if (buffer.size() < 1) return vec;
	swscanf(buffer.c_str(),L"%f %f %f",&vec.X,&vec.Y,&vec.Z);
	return vec;
}

irr::core::position2di CIrrStrParser::ToPosition2i()
{
	position2di vec(0);
	if (buffer.size() < 1) return vec;
	swscanf(buffer.c_str(),L"%d %d",&vec.X,&vec.Y);
	return vec;
}

CPoint2D CIrrStrParser::ToPoint2D()
{
	position2di ps = ToPosition2i();
	CPoint2D out(ps.X,ps.Y);
	return out;
}

irr::core::stringw CIrrStrParser::NextLex(irr::core::stringw delim, bool erase)
{
	stringw out;
	if ((!delim.size()) || (!buffer.size())) return out;
	s32 pos = buffer.find(delim.c_str());
	if (pos < 0) return buffer;
	out = buffer.subString(0,pos,false);
	pos += delim.size();
	if (erase)
		buffer = buffer.subString(pos,buffer.size(),false);
	return out;
}

irrstrwvec CIrrStrParser::ParseToList(irr::core::stringw delim)
{
	irrstrwvec out;
	if (!delim.size()) {
		out.push_back(buffer);
		return out;
	}
	stringw oldbuf = buffer;
	stringw vs;
	do {
		vs = NextLex(delim,true);
		out.push_back(vs);
	} while (vs != buffer);
	buffer = oldbuf;
	oldbuf = "";
	return out;
}

CIrrStrParser & CIrrStrParser::operator += (const irr::core::vector3df & addv)
{
	buffer += addv.X;
	buffer += L" ";
	buffer += addv.Y;
	buffer += L" ";
	buffer += addv.Z;
//	buffer += L" ";
	return *this;
}

CIrrStrParser & CIrrStrParser::operator += (const irr::core::position2di & addp)
{
	buffer += addp.X;
	buffer += L" ";
	buffer += addp.Y;
//	buffer += L" ";
	return *this;
}

CIrrStrParser & CIrrStrParser::operator += (const CPoint2D & addpp)
{
	buffer += addpp.X;
	buffer += L" ";
	buffer += addpp.Y;
//	buffer += L" ";
	return *this;
}

CIrrStrParser::~CIrrStrParser()
{
	// TODO Auto-generated destructor stub
}

} /* namespace gwic */
