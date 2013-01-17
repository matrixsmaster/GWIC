/*
 * CGWIC_Cell.h
 *  Created on: Dec 3, 2012
 *
 *	GWIC (C) Dmitry Soloviov, 2012
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#ifndef CGWIC_CELL_H_
#define CGWIC_CELL_H_

#include <iostream>
#include <string>
#include <irrlicht.h>
#include <irrbullet.h>
#include <bulletworld.h>
#include <rigidbody.h>
#include <boxshape.h>
#include <sphereshape.h>
#include <bvhtrianglemeshshape.h>
#include <trianglemeshshape.h>
#include <gimpactmeshshape.h>
#include <raycastvehicle.h>
#include <collisionobjectaffectordelete.h>
#include <collisionobjectaffectorattract.h>
//#include <collisionobjectaffectorbuoyancy.h>
#include <OALWrapper/OAL_Types.h>
#include <OALWrapper/OAL_Funcs.h>
#include <GWICTypes.h>
#include "CGWIC_Bot.h"
#include <algorithm>

namespace gwic {

class CGWIC_Cell {
public:
	CGWIC_Cell(CPoint2D pos, irr::IrrlichtDevice* dev, irrBulletWorld* phy);
	virtual ~CGWIC_Cell();
	/*
	bool AddActiveActor(CGWIC_Bot* actor);
	CGWIC_Bot* FindActor(int ID);
	void RemoveActor(CGWIC_Bot* actor);
	void RemoveActor(int ID);
	*/
	CPoint2D GetCoord();
	void SetActive(bool on);
	bool GetActive() { return this->active; }
	virtual bool InitLand();
	void SetVisible(const bool enable);
	bool GetVisible() { return this->visible; }
	void DeleteObjects();
	void RandomPlaceObjects(int count, irr::io::path filename);
	irr::core::vector3df getIrrlichtCenter();
	irr::scene::IMesh* TerrainToMesh(int LOD);
	void Update();
	CGWIC_GameObject* PopTransferObject();
	void PushTransferObject(CGWIC_GameObject* optr);
	bool RemoveObjectByNum(irr::u32 num);
	bool RemoveObjectByPtr(CGWIC_GameObject* ptr);
	float GetTerrainHeightUnderPointMetric(irr::core::vector3df pnt);
	//float GetTerrainHeightUnderPointAbsolute(irr::core::vector3df pnt);
	bool SetTerrainHeightUnderPointMetric(irr::core::vector3df pnt, float height, bool update);
	CGWIC_GameObject* GetObjectByIrrPtr(irr::scene::ISceneNode* ptr);
	CGWIC_GameObject* GetObjectByNum(irr::u32 num);
	irr::u32 GetObjectsCount() { return this->objects.size(); }
	void TerrainChanged();
	void RandomizeTerrain(float subdelta);
	void SaveTerrainBitmap();
	void SaveObjectStates();
	void LoadObjectStates();
	void RemoveChangedFlag() { this->terra_changed = false; }
protected:
	int posX;
	int posY;
	float waterLevel;
	float maxHeight;
	float heaven;
	float hell;
	irr::s32 maxLOD;
	irr::scene::E_TERRAIN_PATCH_SIZE maxPATCH;
	irr::s32 terraSmooth;
	bool active;
	bool terra_changed;
	bool visible;
	irr::scene::ITerrainSceneNode* terrain;
	std::vector<irr::scene::ISceneNode*> ournodes;
	std::vector<IRigidBody*> ourphys;
	std::vector<CGWIC_GameObject*> objects;
	ICollisionShape* terra_collision;
	std::vector<CGWIC_GameObject*> transferFIFO;
private:
	irr::IrrlichtDevice* graphics;
	irrBulletWorld* physics;
	irr::scene::ISceneManager* scManager;
	irr::video::IVideoDriver* irDriver;
	irrBulletWorld* phy_world;
};

}

#endif /* CGWIC_CELL_H_ */
