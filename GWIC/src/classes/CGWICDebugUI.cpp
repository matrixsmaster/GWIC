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

void CGWIC_DebugUI::CreateHardcodedUI()
{
	CPoint2D cp(basepoint.X+GWICDEBUG_LOGWIDTH,basepoint.Y+GWICDEBUG_LOGHEIGHT);
	rect<s32> bx(basepoint.X,basepoint.Y,cp.X,cp.Y);
	loglabel = GUI->addStaticText(L"LOG",bx,true);
	cp.X += 16;
	cp.Y = basepoint.Y;
	bx = rect<s32>(cp.X,cp.Y,cp.X+70,cp.Y+20);
	fpslabel = GUI->addStaticText(L"FPS",bx,false);
	cp = basepoint;
	cp.Y += GWICDEBUG_LOGHEIGHT + 4;
	bx = rect<s32>(cp.X,cp.Y,cp.X+GWICDEBUG_LOGWIDTH,cp.Y+20);
	input = GUI->addEditBox(L"",bx,true,NULL,GWIC_GUI_DEBUG_EDITBOX);
	cp.X += bx.getWidth() + 8;
	bx = rect<s32>(cp.X,cp.Y,cp.X+70,cp.Y+20);
	history = GUI->addButton(bx,NULL,GWIC_GUI_DEBUG_EDITBOX+1,L"^",L"History");
	elems.push_back(loglabel);
	elems.push_back(fpslabel);
	elems.push_back(input);
	elems.push_back(history);
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
	history->setVisible(setup);
	//don't hide FPS label :)
	fpslabel->setVisible(true);
	GUI->setFocus(input);
	visible = setup;
}

void CGWIC_DebugUI::HideTotally()
{
	SetVisible(false);
	fpslabel->setVisible(false);
}

void CGWIC_DebugUI::UpdateFPS(int fps)
{
	stringw out = L"FPS: ";
	out += fps;
	fpslabel->setText(out.c_str());
}

void CGWIC_DebugUI::FlushBuffers()
{
	cmdfifo.clear();
	buffer = "";
}

bool CGWIC_DebugUI::PutString(const irr::core::stringw& str)
{
	//FIXME: complete rewrite needed
	CIrrStrParser parse(str);
	if (parse.IsDelimPresent(L">")) {
		stringw to(parse.NextLex(L">",true));
		if (to.equals_ignore_case(L"DBGLOG")) {
			LogText(parse.GetBuff());
			return true;
		}
	}
	LogText(str);
	return true;
}

void CGWIC_DebugUI::PumpMessage(const irr::SEvent& event)
{
	s32 id = event.GUIEvent.Caller->getID();
	if ((id < GWIC_GUI_DEBUG_EDITBOX) || (id >= GWIC_GUI_DEBUG_LAST)) return;
	if (event.GUIEvent.EventType == EGET_EDITBOX_ENTER) {
		stringw buf;
		buf += input->getText();
		if (!buf.empty()) {
			cmdfifo.insert(cmdfifo.begin(),buf);
			histbuf.push_back(buf);
			if (histbuf.size() > GWIC_GUI_DEBUG_HDEPTH)
				histbuf.pop_back();
			input->setText(L"");
		}
	} else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
		if (histbuf.size()) {
			input->setText(histbuf.back().c_str());
			histbuf.pop_back();
		}
		GUI->setFocus(input);
	}
}


} /* namespace gwic */
