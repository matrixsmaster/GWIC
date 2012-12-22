/*
 * CGWIC_World.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Soloviov Dmitry
 */

#ifndef CGWIC_WORLD_H_
#define CGWIC_WORLD_H_

#define GWIC_VOID_COLOR SColor(255,100,100,100)

#include <iostream>
#include <string>
#include <irrlicht.h>
#include <irrbullet.h>
#include <bulletworld.h>
#include <rigidbody.h>
#include <boxshape.h>
#include <sphereshape.h>
#include <gimpactmeshshape.h>
#include <raycastvehicle.h>
#include <collisionobjectaffectordelete.h>
#include <collisionobjectaffectorattract.h>
//#include <collisionobjectaffectorbuoyancy.h>
#include <OALWrapper/OAL_Types.h>
#include <OALWrapper/OAL_Funcs.h>
#include <GWICTypes.h>
#include "CGWIC_Cell.h"
#include "CGWIC_Bot.h"
#include "CGWIC_GameObject.h"
#include "CGWICGUIObject.h"
#include "CGWICDebugUI.h"

namespace gwic {

class CGWIC_World : public irr::IEventReceiver
{
public:
	CGWIC_World(WorldProperties* props, cOAL_Device* sndDevice);
	virtual ~CGWIC_World();
	virtual bool OnEvent(const irr::SEvent& event);
	virtual void RunWorld();
	virtual bool PrepareWorld();
	CGWIC_Cell* GetCell(int x, int y);
//	friend class CGWIC_Cell;
//	friend class CGWIC_Bot;
protected:
	irr::IrrlichtDevice* gra_world;
	irrBulletWorld* phy_world;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* scManager;
	irr::gui::IGUIEnvironment* gui;
	irr::scene::ICameraSceneNode* main_cam;
	std::vector<CGWIC_Cell*> cells;
	std::vector<CGWIC_Bot*> actors;
	std::vector<CGWIC_GameObject*> statobj;
	irr::u32 ticker;
	std::vector<CGWIC_GUIObject*> uis;
	CGWIC_DebugUI* debugui;
	CPoint2D center_cell;
private:
	bool fps_cam;
	bool quit_msg;
	bool debugDraw;
	bool physicsPause;
	WorldProperties properties;
	irr::scene::IBillboardSceneNode* selpoint_bill;
	CPoint2D mousepos;
	float mousewheel;
	bool GenerateLand();
	bool GenerateNPC();
	IRigidBody* ShootSphere(irr::core::vector3df scale, irr::f32 mass);
	void CellTransfers();
//	void GoFPS(irr::core::vector3df campos);
	void GoFPS();
	void GoEditMode();
	void ShowGUI(bool show);
	void ActivateCell(int x, int y);
	float GetTerrainHeightUnderPoint(irr::core::vector3df pnt);
	void ProcessEvents();
	void CommandProcessor(irr::core::stringw cmd);
};

}

#endif /* CGWIC_WORLD_H_ */
