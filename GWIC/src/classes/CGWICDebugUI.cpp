/*
 * CGWICDebugUI.cpp
 *  Created on: Dec 15, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWICDebugUI.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_DebugUI::~CGWIC_DebugUI() {
	// TODO Auto-generated destructor stub
}

void CGWIC_DebugUI::CreateHardcodedUI()
{
	CPoint2D cp(basepoint.X+GWICDEBUG_LOGWIDTH,basepoint.Y+GWICDEBUG_LOGHEIGHT);
	rect<s32> bx(basepoint.X,basepoint.Y,cp.X,cp.Y);
	loglabel = GUI->addStaticText(L"LOG",bx,true);
	cp.X += 16;
	cp.Y = basepoint.Y;
	fpslp = position2di(cp.X,cp.Y);
	bx = rect<s32>(cp.X,cp.Y,cp.X+70,cp.Y+20);
	fpslabel = GUI->addStaticText(L"FPS",bx,false);
	cp = basepoint;
	cp.Y += GWICDEBUG_LOGHEIGHT + 4;
	bx = rect<s32>(cp.X,cp.Y,cp.X+GWICDEBUG_LOGWIDTH,cp.Y+20);
	input = GUI->addEditBox(L"",bx,true,NULL,GWIC_GUI_DEBUG_EDITBOX);
	inppnt = position2di(cp.X,cp.Y);
}

void CGWIC_DebugUI::LogText(irr::core::stringw text)
{
	text += L"\n";
	buffer += text;
	if (buffer.size() > GWICDEBUG_BUFSIZE) {
		int l = buffer.size() - GWICDEBUG_BUFSIZE;
		buffer = buffer.subString(l,buffer.size());
	}
	loglabel->setText(buffer.c_str());
//	const s32 fh = GUI->getSkin()->getFont()->getKerningHeight();
	//while
	if (loglabel->getTextHeight() > GWICDEBUG_LOGHEIGHT) {
		if (buffer.size()) {
			s32 idx = -1;
			wchar_t nl = L'\n';
			idx = buffer.findFirst(nl);
			if (idx < 0) idx = 1;
			else idx++;
			buffer = buffer.subString(idx,buffer.size());
		}// else break;
		loglabel->setText(buffer.c_str());
	}
}

void CGWIC_DebugUI::SetVisible(bool setup)
{
	loglabel->setVisible(setup);
	input->setVisible(setup);
	//don't hide FPS label :)
	visible = setup;
}

void CGWIC_DebugUI::Update()
{
	position2di tmp(basepoint.X,basepoint.Y);
	loglabel->setRelativePosition(tmp);
	fpslabel->setRelativePosition(fpslp+tmp);
	input->setRelativePosition(inppnt+tmp);
}

void CGWIC_DebugUI::UpdateFPS(int fps)
{
	stringw out = L"FPS: ";
	out += fps;
	fpslabel->setText(out.c_str());
}

irr::core::stringw CGWIC_DebugUI::GetNextCommand()
{
	stringw cmd;
	if (cmdfifo.size()) {
		cmd = cmdfifo.back();
		cmdfifo.pop_back();
	}
	return cmd;
}

void CGWIC_DebugUI::FlushBuffers()
{
	cmdfifo.clear();
	buffer = "";
}

void CGWIC_DebugUI::PumpMessage(const irr::SEvent& event)
{
	s32 id = event.GUIEvent.Caller->getID();
	switch (event.GUIEvent.EventType) {
	case EGET_EDITBOX_ENTER:
		if (id == GWIC_GUI_DEBUG_EDITBOX) {
			stringw buf;
			buf += input->getText();
			if (!buf.empty()) {
				cmdfifo.insert(cmdfifo.begin(),buf);
				GUI->getRootGUIElement()->getElementFromId(GWIC_GUI_DEBUG_EDITBOX,true)->setText(L"");
			}
		}
		break;
	default:
		break;
	}
}


} /* namespace gwic */
