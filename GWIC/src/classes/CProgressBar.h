/*
 * CProgressBar.h
 *
 *  Created on: Jan 19, 2013
 *  Based on CProgressBar class by Zurzaza (Irrlicht off. forums)
 *  found here: http://irrlicht.sourceforge.net/forum/viewtopic.php?t=39356
 *
 *  Authors:
 *  	Zurzaza
 *  	matrixsmaster
 */

#ifndef CPROGRESSBAR_H_
#define CPROGRESSBAR_H_

#include <irrlicht.h>

namespace irr {

using namespace core;
using namespace video;

namespace gui {

class CProgressBar: public IGUIElement {
public:
	CProgressBar(IGUIEnvironment* guienv, IGUIElement* parent, const rect<s32>& rectangle, s32 id=-1);

	/*Set percentage in positive percentual (0~100). Please note that
	 *  a call to this function with others values, will set the progress
	 *   bar to 0.
	 */
	void setProgress(u32 progress);

	/*
	 * Set bar Colors
	 */
	void setColors(SColor progress=SColor(255,255,255,255),
			SColor filling=SColor(255,0,0,0));

	/*Allow you to add a "border" into your bar. You MUST specify
	 * the size (of course in pixel) of the border. You can also pass
	 * a color parameter (Black by default)
	 */
	void addBorder(s32 size, SColor color=SColor(255,0,0,0));

	virtual void draw();
private:
	IGUIEnvironment * gui; //GUI ENV. pointer
	s32 total; //Dimension (X) of the bar, to calculate relative percentage.
	rect<s32> bar; //Dimension of the bar
	rect<s32> border; //Border
	rect<s32> tofill; //Percentage indicator
	rect<s32> empty; //"Empty" indicator
	SColor fillcolor;
	SColor emptycolor;
	SColor bordercolor;
};

} /* namespace gui */
} /* namespace irr */
#endif /* CPROGRESSBAR_H_ */
