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
#include "CGWICWindowUI.h"
#include "CGWIC_Gizmo.h"
#include "CGWIC_LoadingScreen.h"
#include "CGWIC_CommandPU.h"
#include "GWIC_SLBinMesh.h"

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
	friend class CGWIC_CommandPU;
	friend class CGWIC_VM;
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
	irr::scene::ILightSceneNode* theSun;
	CGWIC_CommandPU* commander;
private:
	irr::u32 ticker;
	bool fps_cam;
	bool pchar_cam;
	bool quit_msg;
	bool debugDraw;
	bool physicsPause;
	WorldProperties properties;
	CPoint2D mousepos;
	float mousewheel;
	uchar mousepressed;
	irr::scene::ISceneNode* highlited;
	CGWIC_GameObject* selected;
	CGWIC_Bot* select_actor;
	CGWIC_BodyPart* select_actor_part;
	CGWIC_Gizmo* gizmo;
	CGWIC_Bot* PC;
	CPoint2D PC_lastcell;
	bool terrain_magnet;
	bool GenerateLand(CGWIC_LoadingScreen* lscr);
	bool GenerateNPC(CGWIC_LoadingScreen* lscr);
	IRigidBody* ShootSphere(irr::core::vector3df scale, irr::f32 mass);
	void CellTransfers();
	irr::core::vector3df GetCurrentCameraPosition();
	void CameraModeChanged(bool show_gui);
//	void GoFPS(irr::core::vector3df campos);
	void GoFPS();
	void GoEditMode();
	void GoPlayerMode();
	void ShowGUI(bool show);
	void ActivateCell(int x, int y);
	void ActivateCell(CPoint2D trg) { ActivateCell(trg.X,trg.Y); }
	float GetTerrainHeightUnderPointMetric(irr::core::vector3df pnt);
	bool SetTerrainHeightUnderPointMetric(irr::core::vector3df pnt, float height, bool update);
	void ProcessEvents();
	void ProcessSelection();
	void ProcessActors();
	void UpdatePC();
	void ZeroSelect();
	void TerrainMagnet();
	void ReloadCell(CGWIC_Cell* cell);
	void StitchTerrains(CGWIC_Cell* ca, CGWIC_Cell* cb, bool update);
	void StitchWorld(bool lite);
	std::vector<CGWIC_Cell*> GetNeighbors(CPoint2D centr);
	void RemoveRegisteredObject(CGWIC_GameObject* ptr);
	void CreatePlayerCharacter();
	void UpdateHardCulling();
	bool AddLight(CGWIC_GameObject* ptr);
	bool RemoveLight(CGWIC_GameObject* ptr);
	void EraseLights();
	void SunFlick();
	bool CreateNewWindow(irr::io::path filename);
	void CloseAllWindows();
};

static const CPoint2D neighbor_array[] = {
		CPoint2D(0,-1),
		CPoint2D(1,0),
		CPoint2D(0,1),
		CPoint2D(-1,0)
};

}

#endif /* CGWIC_WORLD_H_ */
