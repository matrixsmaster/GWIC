/*
 * CGWICWindowUI.cpp
 *  Created on: Dec 29, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#include "CGWICWindowUI.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

CGWIC_UIWindow::~CGWIC_UIWindow()
{
	acts.clear();
}

bool CGWIC_UIWindow::LoadFromFile(const path filename)
{
	IXMLReader* xml = irDevice->getFileSystem()->createXMLReader(filename);
	if (!xml) {
		std::cerr << "Window description XML file not found!" << std::endl;
		return false;
	}
	std::cout << "Loading UI XML: " << filename.c_str() << std::endl;
	CPoint2D max(0);
	const stringw x_caption(L"Caption");
	const stringw x_button(L"Button");
	const stringw x_editbox(L"EditBox");
	const stringw x_statext(L"Label");
	const stringw x_checkbox(L"CheckBox");
	const stringw x_combobox(L"ComboBox");
	const stringw x_image(L"Image");
	const stringw x_listbox(L"ListBox");
	const stringw x_topmenu(L"TopMenu");
	const stringw x_meshview(L"MeshViewer");
	const stringw x_scrollbar(L"ScrollBar");
	const stringw x_spinbox(L"SpinBox");
	const stringw x_table(L"Table");
	const stringw x_string(L"String");
	IGUIElement* ptr;
	rect<s32> rct;
	vector2di vec;
	CIrrStrParser parse;
	bool border;
	GWICActionPointer cact;
	stringw inside_tag;
	while (xml->read()) {
		if (!inside_tag.empty()) {
			if (xml->getNodeType() == EXN_ELEMENT) {
				if ((inside_tag.equals_ignore_case(x_listbox)) &&
						(x_string.equals_ignore_case(xml->getNodeName()))) {
					IGUIListBox* lptr = reinterpret_cast<IGUIListBox*> (elems.back());
					lptr->addItem(xml->getAttributeValueSafe(L"text"));
				}
			} else if (xml->getNodeType() == EXN_ELEMENT_END)
				inside_tag = L"";
			continue;
		}
		parse = xml->getAttributeValueSafe(L"pos");
		vec.X = parse.ToPoint2D().X;
		vec.Y = parse.ToPoint2D().Y;
		rct.UpperLeftCorner = vec;
		parse = xml->getAttributeValueSafe(L"size");
		rct.LowerRightCorner = vec + vector2d<s32>(parse.ToPoint2D().X,parse.ToPoint2D().Y);
		if (rct.LowerRightCorner.X > max.X) max.X = rct.LowerRightCorner.X;
		if (rct.LowerRightCorner.Y > max.Y) max.Y = rct.LowerRightCorner.Y;
		border = (xml->getAttributeValueAsInt(L"border"));
		ptr = NULL;
		cact.accept_filter = xml->getAttributeValueSafe(L"filter");
		cact.command = xml->getAttributeValueSafe(L"cmd");
		cact.init_done = false;
		cact.type.clear();
		if (x_caption.equals_ignore_case(xml->getNodeName())) {
			window->setText(xml->getAttributeValueSafe(L"text"));
		} else if (x_button.equals_substring_ignore_case(xml->getNodeName())) {
			ptr = GUI->addButton(rct,window,IterateID(),xml->getAttributeValueSafe(L"text"),
					xml->getAttributeValueSafe(L"tip"));
			cact.type.push_back(EGET_BUTTON_CLICKED);
			cact.init_done = true;
		} else if (x_editbox.equals_substring_ignore_case(xml->getNodeName())) {
			ptr = GUI->addEditBox(xml->getAttributeValueSafe(L"text"),rct,border,window,IterateID());
		} else if (x_statext.equals_substring_ignore_case(xml->getNodeName())) {
			ptr = GUI->addStaticText(xml->getAttributeValueSafe(L"text"),rct,border,true,
					window,IterateID(),(xml->getAttributeValueAsInt(L"fill")));
		} else if (x_checkbox.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_combobox.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_image.equals_substring_ignore_case(xml->getNodeName())) {
			ITexture* txd = irDevice->getVideoDriver()->getTexture(xml->getAttributeValueSafe(L"file"));
			if (txd)
				ptr = GUI->addImage(txd,rct.UpperLeftCorner,true,window,IterateID());
			else {
				std::cerr << "Image could't be loaded! " << xml->getAttributeValueSafe(L"file");
				std::cerr << std::endl;
			}
		} else if (x_listbox.equals_substring_ignore_case(xml->getNodeName())) {
			ptr = GUI->addListBox(rct,window,IterateID(),(xml->getAttributeValueAsInt(L"back")));
			if (!xml->isEmptyElement()) inside_tag = xml->getNodeName();
		} else if (x_topmenu.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_meshview.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_scrollbar.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_spinbox.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		} else if (x_table.equals_substring_ignore_case(xml->getNodeName())) {
			//TODO
		}
		if (ptr) {
			cact.ptr = ptr;
			elems.push_back(ptr);
			cact.gid = ptr->getID();
//			if (cact.init_done)
				acts.push_back(cact);
		} else inside_tag = L"";
	}
	if (max.X < GWIC_GUI_WINDOW_MIN) max.X = GWIC_GUI_WINDOW_MIN;
	if (max.Y < GWIC_GUI_WINDOW_MIN) max.Y = GWIC_GUI_WINDOW_MIN;
	max += GWIC_GUI_WINDOW_LOWBORDER;
	xml->drop();
	window->setMinSize(dimension2du(max.X,max.Y));
	return true;
}

void CGWIC_UIWindow::PumpMessage(const irr::SEvent& event)
{
	s32 id = event.GUIEvent.Caller->getID();
	u32 j;
	for (u32 i=0; i<acts.size(); i++) {
		if (acts[i].gid == id) {
			for (j=0; j<acts[i].type.size(); j++) {
				if (event.GUIEvent.EventType == acts[i].type[j]) {
					std::cout << "Event received!" << std::endl;
					ProcessAction(i,j);
				}
			}
		}
	}
}

void CGWIC_UIWindow::Update()
{
	//
}

bool CGWIC_UIWindow::PutString(const irr::core::stringw str)
{
	//TODO: add "accept" rule to absorb the text and set gui element text
	return false;
}

void CGWIC_UIWindow::SetPos(CPoint2D nwpos)
{
	basepoint = nwpos;
	if (basepoint.X < 0) basepoint.X = 0;
	if (basepoint.Y < 0) basepoint.Y = 0;
	window->setRelativePosition(position2di(basepoint.X,basepoint.Y));
}

void CGWIC_UIWindow::ProcessAction(uint action, uint type)
{
	stringw cmd = acts[action].command;
	if (!cmd.empty()) {
		if (cmd.find(L"$") >= 0) {
			//command extend
			CIrrStrParser parse(cmd);
			stringw eprfx = parse.NextLex(L"$",true);
			stringw etype = parse.NextLex(L".",true);
			stringw ename = parse.NextLex(L" ",true);
			stringw res;
			for (u32 i=0; i<acts.size(); i++) {
				if ( (!acts[i].accept_filter.empty()) && (acts[i].accept_filter == ename) ) {
					if (etype == L"text") {
						res = acts[i].ptr->getText();
					}
				}
			}
			cmd = eprfx;
			cmd += res;
			cmd += parse.GetBuff();
		}
		cmdfifo.insert(cmdfifo.begin(),cmd);
	}
}


} /* namespace gwic */
