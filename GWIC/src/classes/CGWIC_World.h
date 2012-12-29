/*
 * CGWIC_World.h
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
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
	CGWIC_Cell* GetCell(CPoint2D trg) { return (GetCell(trg.X,trg.Y)); }
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
	std::vector<CGWIC_GUIObject*> uis;
	CGWIC_DebugUI* debugui;
	CPoint2D center_cell;
private:
	irr::u32 ticker;
	bool fps_cam;
	bool quit_msg;
	bool debugDraw;
	bool physicsPause;
	WorldProperties properties;
	irr::scene::IBillboardSceneNode* selpoint_bill;
	CPoint2D mousepos;
	float mousewheel;
	bool mousepressed;
//	int mousebutton;
	irr::scene::ISceneNode* highlited;
	CGWIC_GameObject* selected;
	CGWIC_Bot* select_actor;
	CGWIC_BodyPart* select_actor_part;
	bool terrain_magnet;
	bool GenerateLand();
	bool GenerateNPC();
	IRigidBody* ShootSphere(irr::core::vector3df scale, irr::f32 mass);
	void CellTransfers();
//	void GoFPS(irr::core::vector3df campos);
	void GoFPS();
	void GoEditMode();
	void ShowGUI(bool show);
	void ActivateCell(int x, int y);
	void ActivateCell(CPoint2D trg) { ActivateCell(trg.X,trg.Y); }
	float GetTerrainHeightUnderPointMetric(irr::core::vector3df pnt);
	bool SetTerrainHeightUnderPointMetric(irr::core::vector3df pnt, float height, bool update);
	void ProcessEvents();
	void ProcessSelection();
	void ProcessActors();
	void ZeroSelect();
	void CommandProcessor(irr::core::stringw cmd);
	void CmdGetPos(CIrrStrParser parse);
	void TerrainMagnet();
	void ReloadCell(CGWIC_Cell* cell);
	void StitchTerrains(CGWIC_Cell* ca, CGWIC_Cell* cb, bool update);
	void StitchWorld(bool lite);
	std::vector<CGWIC_Cell*> GetNeighbors(CPoint2D centr);
};

static const CPoint2D neighbor_array[] = {
		CPoint2D(0,-1),
		CPoint2D(1,0),
		CPoint2D(0,1),
		CPoint2D(-1,0)
};

}

#endif /* CGWIC_WORLD_H_ */
