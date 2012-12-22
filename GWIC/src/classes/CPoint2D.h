/*
 * CPoint2D.h
 *  Created on: 06.12.2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CPOINT2D_H_
#define CPOINT2D_H_

namespace gwic {

class CPoint2D {
public:
	CPoint2D() : X(0), Y(0) {}
	CPoint2D(int x, int y) : X(x), Y(y) {}
	CPoint2D(int both) : X(both), Y(both) {}
	CPoint2D(const CPoint2D & nw) : X(nw.X), Y(nw.Y) {}
	//virtual ~CPoint2D();
	CPoint2D & operator = (const CPoint2D & other) { X=other.X; Y=other.Y; return *this; }
	CPoint2D operator + (const CPoint2D & add) const {return (CPoint2D(X+add.X), CPoint2D(Y+add.Y));}
	bool operator == (const CPoint2D & comp) const {return ((X==comp.X) && (Y==comp.Y));}
	int X;
	int Y;
};

} /* namespace gwic */
#endif /* CPOINT2D_H_ */
