/*
 * CGWIC_World.cpp
 *  Created on: Dec 3, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 *
 */

#include "CGWIC_World.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace gwic {

bool CGWIC_World::OnEvent(const irr::SEvent& event)
{
	if (!gra_world) return false;
	switch(event.EventType) {
	case EET_MOUSE_INPUT_EVENT:
		mousepressed = 0;
		if (event.MouseInput.isLeftPressed()) mousepressed = 1;
		if (event.MouseInput.isRightPressed()) mousepressed |= 2;
		if (event.MouseInput.isMiddlePressed()) mousepressed |= 4;
		if (gizmo) {
			gizmo->ProcessEvent(event);
			if (mousepressed) {
				if (select_actor_part)
					//FIXME: this is just a test. We need apply a force with a vector drawn by mouse instead of direct movement
					select_actor_part->Move(gizmo->GetDifference());
				else if (select_actor) {
					select_actor->SetCell(gizmo->GetCurrentCell());
					select_actor->SetPos(gizmo->GetCellRelativePosMetric());
				} else if (selected) {
					selected->SetCell(gizmo->GetCurrentCell());
					selected->SetPos(gizmo->GetCellRelativePosMetric());
				}
			}
		}
		mousepos.X = event.MouseInput.X;
		mousepos.Y = event.MouseInput.Y;
		mousewheel = event.MouseInput.Wheel;
		//absorb event if selection is active
		if ((mousepressed) && (gizmo)) return true;
		break;
	case EET_KEY_INPUT_EVENT:
		//FIXME: more robust and nice looking processing needed!!
		if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown == false) {
//			fps_cam ^= true;
			if (!fps_cam) GoFPS();
			else GoEditMode();
			return true;
		}
		if (!fps_cam) break;
		if (event.KeyInput.Key == KEY_KEY_Q && event.KeyInput.PressedDown == false) {
			quit_msg = true;
			return true;
		}
		if (event.KeyInput.Key == KEY_SPACE && event.KeyInput.PressedDown == false) {
			if (fps_cam) {
				float ssize = GWIC_IRRUNITS_PER_METER / 4.f;
				this->ShootSphere(vector3df(ssize,ssize,ssize),3.0);
			}
			return true;
		}
		if (event.KeyInput.Key == KEY_DELETE && event.KeyInput.PressedDown == false) {
			std::cout << "Deleting main camera!" << std::endl;
			main_cam->remove();
			main_cam = NULL;
			return true;
		}
		if (event.KeyInput.Key == KEY_F9 && event.KeyInput.PressedDown == false) {
			physicsPause ^= true;
			if (physicsPause) debugui->LogText(L"Physics paused");
			else debugui->LogText(L"Physics resumed");
			return true;
		}
		if (event.KeyInput.Key == KEY_F8 && event.KeyInput.PressedDown == false) {
			terrain_magnet ^= true;
			if (terrain_magnet) {
				debugui->LogText(L"Terrain magnet activated!");
				std::cout << "Terrain magnet activated!" << std::endl;
			} else
				StitchWorld(false);
			return true;
		}
		break;
	case EET_GUI_EVENT:
		if (debugui) debugui->PumpMessage(event);
		if (event.GUIEvent.EventType == EGET_ELEMENT_CLOSED) {
			std::vector<CGWIC_GUIObject*>::iterator uisit = uis.begin();
			CGWIC_GUIObject* ptr;
			for (;uisit != uis.end(); ++uisit) {
				ptr = *uisit;
				if (ptr->GetRootID() == event.GUIEvent.Caller->getID()) {
					delete ptr;
					uis.erase(uisit);
					break;
				}
			}
			return true;
		} else {
			for (u32 i=0; i<uis.size(); i++)
				uis[i]->PumpMessage(event);
		}
		break;
	default: break;
	}
	return false;
}

CGWIC_World::CGWIC_World(WorldProperties* props, cOAL_Device* sndDevice)
{
	gra_world = NULL;
	phy_world = NULL;
	std::cout << "Hello from world constructor!" << std::endl;
	quit_msg = false;
	properties = *props;
	debugDraw = props->debugdraw;
	physicsPause = true;
	selpoint_bill = NULL;
	debugui = NULL;
	highlited = NULL;
	select_actor = NULL;
	select_actor_part = NULL;
	gizmo = NULL;
	terrain_magnet = false;

	std::cout << "Creating Irrlicht device..." << std::endl;
	gra_world = createDevice(props->videoDriver,dimension2d<u32>(props->gWidth,props->gHeight),
							props->bits,props->fullscreen,props->stencil,props->vsync,this);
	if (!gra_world) {
		std::cerr << "Irrlicht device creation FAILED" << std::endl;
		return;
	}
	gra_world->setWindowCaption(GWIC_MAINWINDOW_CAPTION);
	driver = gra_world->getVideoDriver();
	scManager = gra_world->getSceneManager();
	gui = gra_world->getGUIEnvironment();
	driver->beginScene(true,true,GWIC_VOID_COLOR);
	driver->endScene(); //we draw the empty scene to ensure our window is filled
	gra_world->getFileSystem()->addFileArchive(props->vfsRoot,true,false,props->vfsType);

	std::cout << "Enabling physics..." << std::endl;
	phy_world = createIrrBulletWorld(gra_world,true,debugDraw);
	if (!phy_world) {
		std::cerr << "Physics enabling FAILED" << std::endl;
		return;
	}
	phy_world->setDebugMode(EPDM_DrawAabb | EPDM_DrawContactPoints);
	phy_world->setGravity(vector3df(0,props->Gforce,0));
}

CGWIC_World::~CGWIC_World()
{
	std::cout << "Hello from the world DESTRUCTOR!!" << std::endl;
	while (cells.size() > 0) {
		delete (cells.back());
		cells.pop_back();
	}
	while (actors.size() > 0) {
		delete (actors.back());
		actors.pop_back();
	}
	if (phy_world) delete phy_world;
	if (gra_world) gra_world->drop();
}

CGWIC_Cell* CGWIC_World::GetCell(int x, int y)
{
	if ((x<0) || (y<0)) {
		std::cerr << "GetCell() X or Y below zero" << std::endl;
		return NULL;
	}
	if ((x>=properties.wrldSizeX) || (y>=properties.wrldSizeY)) {
		std::cerr << "GetCell() X or Y above world size limits!" << std::endl;
		return NULL;
	}
	// pass 1
	unsigned int r = x * properties.wrldSizeX + y;
	CPoint2D ccrd;
	CPoint2D targ(x,y);
	if (r<cells.size()) {
		ccrd = cells[r]->GetCoord();
		if (ccrd == targ) {
//			std::cout << "Cell search complete in first phase!" << std::endl;
			return (cells[r]);
		}
	}
	// pass 2
	std::cerr << "Search for cell [" << x;
	std::cerr << ";" << y;
	std::cerr << "] failed on first phase. Doing bruteforce search!" << std::endl;
	for (r=0; r<cells.size(); r++) {
		ccrd = cells[r]->GetCoord();
		if (ccrd == targ) {
			std::cout << "Cell search complete on second phase!" << std::endl;
			return (cells[r]);
		}
	}
	std::cerr << "Search for cell [" << x;
	std::cerr << ";" << y;
	std::cerr << "] failed on second phase! CELL buffer lost some data!!!" << std::endl;
	return NULL;
}

bool CGWIC_World::GenerateLand()
{
	std::cout << "World width (cells): " << properties.wrldSizeX;
	std::cout << std::endl << "World height (cells): ";
	std::cout << properties.wrldSizeY << std::endl;
	CGWIC_Cell* ptr;
	int y;
	for (int x=0; x<properties.wrldSizeX; x++)
		for (y=0; y<properties.wrldSizeY; y++) {
			ptr = new CGWIC_Cell(CPoint2D(x,y),gra_world,phy_world);
			cells.push_back(ptr);
			if (!ptr->InitLand()) return false;
		}
	StitchWorld(true);
	return true;
}

bool CGWIC_World::GenerateNPC()
{
	BotCreationParams botset;
	botset.type = ACTOR_DUMMY;
	botset.cell_coord = CPoint2D(2);
	botset.rel_pos = vector3df(120,50,120);
	botset.height = 1.5f;
	botset.actorname = L"Sydney";
	actors.push_back(new CGWIC_Bot(&botset,gra_world,phy_world));
	actors.back()->SetEnabled(true);
	botset.type = ACTOR_GYNOID;
	botset.rel_pos = vector3df(110,50,110);
	botset.actorname = L"A22";
	actors.push_back(new CGWIC_Bot(&botset,gra_world,phy_world));
	//actors.back()->SetEnabled(true);
	return true;
}

bool CGWIC_World::PrepareWorld()
{
	if ((!gra_world) || (!phy_world)) return false;
	std::cout << "Beware!!! Overlord is here!!" << std::endl;
	std::cout << "World preparation begins..." << std::endl;

	//Create landscape
	if (GenerateLand()) std::cout << "Land generated!" << std::endl;
	else {
		std::cerr << "Error while generating land!" << std::endl;
		return false;
	}

	//Create actors
	if (GenerateNPC()) std::cout << "NPCs generated successfully!" << std::endl;
	else {
		std::cerr << "Error while generating NPCs!" << std::endl;
		return false;
	}

	//Activate start location
	std::cout << "Activating start cell!" << std::endl;
	ActivateCell(properties.startX,properties.startY);
	//GetCell(0,0)->SetActive(false);

	//Initialize the camera
	std::cout << "Creating initial camera" << std::endl;
	main_cam = NULL;
	GoEditMode();
	if (!main_cam) return false;
	selpoint_bill = scManager->addBillboardSceneNode();
	selpoint_bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR );
	selpoint_bill->setMaterialTexture(0,driver->getTexture("particlered.bmp"));
	selpoint_bill->setMaterialFlag(EMF_LIGHTING,false);
	selpoint_bill->setMaterialFlag(EMF_ZBUFFER,false);
	selpoint_bill->setSize(dimension2d<f32>(GWIC_IRRUNITS_PER_METER,GWIC_IRRUNITS_PER_METER));
	selpoint_bill->setID(-111);

	//Prepare sky
	std::cout << "Prepare atmosphere" << std::endl;
	//FIXME: we need to use some overlays with clouds and something
	//here'll be a some custom class, but not now :)
	scManager->addSkyDomeSceneNode(driver->getTexture("skydome.jpg"),16,8,0.95f,2.0f);
	scManager->setAmbientLight(SColorf(0.3,0.3,0.6));

	//Initialize font and skin
	std::cout << "Initializing GUI skin" << std::endl;
	IGUISkin* skin = gui->getSkin();
	IGUIFont* gfont = gui->getFont("fonthaettenschweiler.bmp");
	if (gfont) skin->setFont(gfont);
	else std::cerr << "Font not found!" << std::endl;

	//Add debug UI
	//note: debugUI must be the first UI object created!
	std::cout << "Creating debug UI" << std::endl;
	debugui = new CGWIC_DebugUI(gra_world);
	if (!debugui) return false;
	debugui->SetPos(CPoint2D(150,16));

	//we're done with it :)
	return true;
}

void CGWIC_World::RunWorld()
{
	if ((!gra_world) || (!phy_world)) return;
	//TODO: get this threaded
	ITimer* timer = gra_world->getTimer();
	u32 TimeStamp = timer->getTime();
	u32 DeltaTime = 0;
	int lFPS = -1;
	int cFPS;
	ticker = 0;
	while (gra_world->run()) {
		ticker++;
		if (!gra_world->isWindowActive()) {
			gra_world->yield();
			continue;
		}
		if (ticker%5) ProcessEvents();
		DeltaTime = timer->getTime() - TimeStamp;
		TimeStamp = timer->getTime();
		driver->beginScene(true,true,GWIC_VOID_COLOR);

		//FIXME: move physics simulation to another thread
		if (!physicsPause)
			phy_world->stepSimulation(DeltaTime*0.001f,120);
		if (debugDraw) {
			phy_world->debugDrawWorld(true);
			phy_world->debugDrawProperties(true);
		}

		scManager->drawAll();

		if ((main_cam) && (mousepressed)) ProcessSelection();

		gui->drawAll();
		driver->endScene();
		if (quit_msg) {
			std::cerr << "Exit with quit_msg flag!" << std::endl;
			break;
		}

		cFPS = driver->getFPS();
		if (cFPS != lFPS) {
			lFPS = cFPS;
			debugui->UpdateFPS(cFPS);
		}

		ProcessActors(); //FIXME: separate thread

		//FIXME: need more robust update scheduling (on time, and in separate thread)
		if (ticker > 25) {
			ticker = 0;
			for (u32 i=0; i<cells.size(); i++)
				cells[i]->Update();
			CellTransfers();
		}
	}
	if (!quit_msg)
		std::cerr << "Exit without quit_msg flag!" << std::endl;
	std::cout << "RunWorld() done!" << std::endl;
}

IRigidBody* CGWIC_World::ShootSphere(irr::core::vector3df scale, irr::f32 mass)
{
	//this function is from irrBullet's tutorial, just for debugging early pre-pre-pre-alphas
	vector3df pos(main_cam->getPosition().X,main_cam->getPosition().Y,main_cam->getPosition().Z);
	ISceneNode *Node = scManager->addSphereSceneNode(5,16,NULL,GWIC_PICKABLE_MASK);
	ITriangleSelector* sel = scManager->createTriangleSelectorFromBoundingBox(Node);
	Node->setTriangleSelector(sel);
	sel->drop();
	Node->setScale(scale);
	Node->setPosition(pos);
	Node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
	Node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, driver->getTexture("stones.jpg"));
	ICollisionShape *shape = new ISphereShape(Node, mass, true);
	IRigidBody *body = phy_world->addRigidBody(shape);
	body->setDamping(0.1,0.1);
    // Since we use a "bumpy" triangle mesh for the terrain, we need to set the CCD radius and
    // motion threshold to ensure that we don't have any tunneling problems.
    // This doesn't work very well for box shapes.
	// ALSO: If you use setLocalScaling() on the collision shape, be sure to call this again
	// after scaling to make sure that it matches the actual object.
	vector3df rot = main_cam->getRotation();
    matrix4 mat;
    mat.setRotationDegrees(rot);
    vector3df forwardDir(vector3df(mat[8],mat[9],mat[10]) * 120);
    body->setLinearVelocity(forwardDir);
    //ICollisionObjectAffectorDelete *deleteAffector = new ICollisionObjectAffectorDelete(4000);
    //body->addAffector(deleteAffector);
    return body;
}

void CGWIC_World::CellTransfers()
{
	CGWIC_Cell* cptr;
	CGWIC_GameObject* optr;
	CPoint2D targ;
	for (u32 i=0; i<cells.size(); i++) {
		do {
			optr = cells[i]->PopTransferObject();
			if (optr) {
				targ = optr->GetCell();
				cptr = GetCell(targ);
				if (!cptr) {
					std::cerr << "Object goes off world! [" << targ.X << ";";
					std::cerr << targ.Y << "]" << std::endl;
					delete optr;
				} else
					cptr->PushTransferObject(optr);
			}
		} while (optr != NULL);
	}
}

void CGWIC_World::GoFPS()
{
	vector3df oldpos(0);
	if (main_cam) {
		main_cam->remove();
		oldpos = (fps_cam)? main_cam->getPosition():main_cam->getTarget();
	} else {
		CGWIC_Cell* ptr = GetCell(center_cell);
		oldpos = ptr->getIrrlichtCenter();
		oldpos.Y = ptr->GetTerrainHeightUnderPointMetric(oldpos) * GWIC_IRRUNITS_PER_METER;
	}
	std::cout << "Camera set to First-Person mode" << std::endl;
	if (debugui) debugui->LogText(L"Camera set to First-Person mode");
	main_cam = scManager->addCameraSceneNodeFPS();
	main_cam->setPosition(oldpos);
	main_cam->setFarValue(properties.viewDistance * GWIC_IRRUNITS_PER_METER);
	ShowGUI(false);
	ZeroSelect();
	fps_cam = true;
}

void CGWIC_World::GoEditMode()
{
	vector3df oldpos(0);
	if (main_cam) {
		main_cam->remove();
		oldpos = (fps_cam)? main_cam->getPosition():main_cam->getTarget();
	} else {
		CGWIC_Cell* ptr = GetCell(center_cell);
		oldpos = ptr->getIrrlichtCenter();
		oldpos.Y = ptr->GetTerrainHeightUnderPointMetric(oldpos) * GWIC_IRRUNITS_PER_METER;
	}
	if (selected) oldpos = selected->GetRootNode()->getAbsolutePosition();
	std::cout << "Camera set to Maya mode" << std::endl;
	if (debugui) debugui->LogText(L"Camera set to Maya mode");
	main_cam = scManager->addCameraSceneNodeMaya();
	main_cam->setTarget(oldpos);
	main_cam->setFarValue(properties.viewDistance * GWIC_IRRUNITS_PER_METER);
	ShowGUI(true);
	ZeroSelect();
	fps_cam = false;
}

void CGWIC_World::ShowGUI(bool show)
{
	if (debugui) debugui->SetVisible(show);
}

void CGWIC_World::ActivateCell(int x, int y)
{
	int cx,cy;
	/* Next code not working due to bug in Irrlicht, that prevents
	 * us of detecting more terrain collisions than designed
	 * (one terrain in a point near (0,Y,0)
	 * Even if you totally destroy older TriangleSelectors,
	 * there's no way (in licht 1.8) to use TS for terrain anymore (sic!)
	CGWIC_Cell* ptr = GetCell(x,y);
	if (!ptr) {
		std::cerr << "Activation main cell not found!" << std::endl;
		quit_msg = true;
		return;
	}
	//maybe this can be workaround for terrain collisions :-|
	if (ptr->GetActive()) ptr->SetActive(false); // deactivate it first
	ptr->SetActive(true);
	for (u32 i=0; i<cells.size(); i++) {
		cx = cells[i]->GetCoord().X;
		cy = cells[i]->GetCoord().Y;
		if ((cx>x-2) && (cy>y-2) && (cx<x+2) && (cy<y+2) && ((cx!=x) || (cy!=y))) {
			if (!(cells[i]->GetActive())) cells[i]->SetActive(true);
		} else if ((cx!=x) && (cy!=y)) {
			if (cells[i]->GetActive()) cells[i]->SetActive(false);
		}
	}*/
	// So, we use this:
	for (u32 i=0; i<cells.size(); i++) {
		cx = cells[i]->GetCoord().X;
		cy = cells[i]->GetCoord().Y;
		if ((cx>x-2) && (cy>y-2) && (cx<x+2) && (cy<y+2)) {
			if (!(cells[i]->GetActive())) cells[i]->SetActive(true);
		} else {
			if (cells[i]->GetActive()) cells[i]->SetActive(false);
		}
	}
	center_cell.X = x;
	center_cell.Y = y;
}

float CGWIC_World::GetTerrainHeightUnderPointMetric(irr::core::vector3df pnt)
{
	pnt /= GWIC_IRRUNITS_PER_METER;
	const s32 cx = static_cast<s32> (pnt.X/GWIC_METERS_PER_CELL);
	const s32 cy = static_cast<s32> (pnt.Z/GWIC_METERS_PER_CELL);
	CGWIC_Cell* cellptr = GetCell(cx,cy);
	if (cellptr) {
		pnt.X -= GWIC_METERS_PER_CELL * cx;
		pnt.Z -= GWIC_METERS_PER_CELL * cy;
		//workaround due to different size of our cells and irrlicht's heightmaps
		pnt.X = pnt.X * 256 / GWIC_METERS_PER_CELL;
//		if (pnt.X > GWIC_METERS_PER_CELL) pnt.X = 255;
		pnt.Z = pnt.Z * 256 / GWIC_METERS_PER_CELL;
//		if (pnt.Z > GWIC_METERS_PER_CELL) pnt.Z = 255;
//		std::cout << "get point: " << pnt.X << " : " << pnt.Z << std::endl;
		return (cellptr->GetTerrainHeightUnderPointMetric(pnt));
	}
	return 0;
}

bool CGWIC_World::SetTerrainHeightUnderPointMetric(irr::core::vector3df pnt, float height, bool update)
{
	pnt /= GWIC_IRRUNITS_PER_METER;
	const s32 cx = static_cast<s32> (pnt.X/GWIC_METERS_PER_CELL);
	const s32 cy = static_cast<s32> (pnt.Z/GWIC_METERS_PER_CELL);
	CGWIC_Cell* cellptr = GetCell(cx,cy);
	if (cellptr) {
		pnt.X -= GWIC_METERS_PER_CELL * cx;
		pnt.Z -= GWIC_METERS_PER_CELL * cy;
		//workaround due to different size of our cells and irrlicht's heightmaps
		pnt.X = pnt.X * 256 / GWIC_METERS_PER_CELL;
//		if (pnt.X > GWIC_METERS_PER_CELL) pnt.X = 255;
		pnt.Z = pnt.Z * 256 / GWIC_METERS_PER_CELL;
//		if (pnt.Z > GWIC_METERS_PER_CELL) pnt.Z = 255;
//		std::cout << "set:" << cx << ':' << cy << " :+ " << pnt.X << " : " << pnt.Z << std::endl;
		return (cellptr->SetTerrainHeightUnderPointMetric(pnt,height,update));
	} else
		return false;
}

void CGWIC_World::ProcessEvents()
{
	if ((main_cam) && (terrain_magnet)) TerrainMagnet();
	stringw cmdstr;
	while (!(cmdstr = debugui->GetNextCommand()).empty())
		CommandProcessor(cmdstr);
	for (u32 i=0; i<uis.size(); i++) {
		while (!(cmdstr = uis[i]->GetNextCommand()).empty())
			CommandProcessor(cmdstr);
	}
}

void CGWIC_World::ProcessSelection()
{
	ISceneCollisionManager* pmgr = scManager->getSceneCollisionManager();
	line3d<f32> ray;
	vector3df rayhit;
	triangle3df hit_triag;
	CGWIC_Cell* cellptr;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	highlited = NULL;
	if (fps_cam) {
		ray.start = main_cam->getPosition();
		ray.end = ray.start + (main_cam->getTarget() - ray.start).normalize() * main_cam->getFarValue();
	} else
		ray = pmgr->getRayFromScreenCoordinates(vector2di(mousepos.X,mousepos.Y),main_cam);
	highlited = pmgr->getSceneNodeAndCollisionPointFromRay(ray,rayhit,hit_triag,GWIC_PICKABLE_MASK,0);
	if (highlited) {
		cellptr = GetCell((rayhit.X/dim),(rayhit.Z/dim));
		if (cellptr) {
			//is this an object in cell?
			selected = cellptr->GetObjectByIrrPtr(highlited);
		}
		if (!selected) {
			//maybe this is an actor or part?
			u32 i,j;
			for (i=0; i<actors.size(); i++) {
				j = actors[i]->IsThisNodeIsMine(highlited);
				if (j) {
					select_actor = actors[i];
					stringc nm = select_actor->GetName();
					std::cout << "Actor selected: " << nm.c_str() << std::endl;
					if (j > 1) {
						select_actor_part = select_actor->GetHead()->GetBPbyNode(highlited);
						if (select_actor_part) {
							std::cout << nm.c_str() << "'s BP selected: " << select_actor_part->GetName().c_str();
							std::cout << std::endl;
						}
					}
					break;
				}
			}
		}
		if (selpoint_bill) selpoint_bill->setPosition(rayhit);
		if (gizmo) gizmo->SetAbsolutePosition(rayhit);
		else gizmo = new CGWIC_Gizmo(gra_world,rayhit);
	} else
		ZeroSelect();
}

void CGWIC_World::ProcessActors()
{
	//TODO: fix actors fall through terrain
	//TODO: process brains
}

void CGWIC_World::ZeroSelect()
{
	if (selpoint_bill) selpoint_bill->setPosition(vector3df(0));
	selected = NULL;
	highlited = NULL;
	select_actor = NULL;
	select_actor_part = NULL;
	if (gizmo) delete gizmo;
	gizmo = NULL;
}

void CGWIC_World::CommandProcessor(irr::core::stringw cmd)
{
	stringc cons = cmd.c_str();
	std::cout << "Command processor: cmd: " << cons.c_str() << std::endl;
	CIrrStrParser parse(cmd);
	stringw icmd = parse.NextLex(L" ",false);
	if (icmd == L"quit") {
		quit_msg = true;
	} else if (icmd == L"test") {
		debugui->LogText(L"Test Response String");
		irrstrwvec list = parse.ParseToList(L" ");
		for (u32 i=1; i<list.size(); i++) {
			stringw bvs = L"Argument: ";
			bvs += list[i];
			debugui->LogText(bvs);
		}
	} else if (icmd == L"getpos") {
		CmdGetPos(parse);
	} else if (icmd == L"randomplace") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 3) {
			debugui->LogText(L"Insufficient arguments");
			return;
		}
		CGWIC_Cell* ptr = GetCell(center_cell);
		CIrrStrParser pr2(list[2]);
		ptr->RandomPlaceObjects(pr2.ToS32(),list[1]);
	} else if (icmd == L"tpm") {
		debugui->LogText(L"toggle physics mode");
		physicsPause ^= true;
	} else if (icmd == L"tdd") {
		debugui->LogText(L"toggle debug drawing");
		debugDraw ^= true;
	} else if (icmd == L"reloadcell") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 3) {
			debugui->LogText(L"Insufficient args. Use: reloadcell X Y");
			return;
		}
		CIrrStrParser pr2(list[1]);
		s32 clx = pr2.ToS32(); pr2 = list[2];
		s32 cly = pr2.ToS32();
		CGWIC_Cell* clptr = GetCell(clx,cly);
		if (!clptr) debugui->LogText(L"Cell not found!");
		else ReloadCell(clptr);
	} else if (icmd == L"stitch") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 5) {
			debugui->LogText(L"Insufficient args. Use: stitch Xa Ya Xb Yb");
			return;
		}
		CIrrStrParser pr2(list[1]);
		CPoint2D aa,bb;
		aa.X = pr2.ToS32();
		pr2 = list[2]; aa.Y = pr2.ToS32();
		pr2 = list[3]; bb.X = pr2.ToS32();
		pr2 = list[4]; bb.Y = pr2.ToS32();
		StitchTerrains(GetCell(aa),GetCell(bb),true);
		debugui->LogText(L"Command done!");
	} else if (icmd == L"restitch") {
		StitchWorld(false);
		debugui->LogText(L"Command done!");
	} else if (icmd == L"createwindow") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 2) {
			debugui->LogText(L"Use: createwindow <filename.xml>");
			return;
		}
		CGWIC_UIWindow* ptr = new CGWIC_UIWindow(gra_world);
		if (!ptr)
			std::cerr << "UI window creation failed!" << std::endl;
		else {
			if ((uis.size()) && (uis.back()))
				ptr->SetNextID(uis.back()->IterateID());
			else
				ptr->SetNextID(GWIC_GUI_DEBUG_LAST);
			uis.push_back(ptr);
			ptr->LoadFromFile(list[1]);
			debugui->LogText(L"Command done!");
		}
	} else if (icmd == L"randomterr") {
		irrstrwvec list = parse.ParseToList(L" ");
		if (list.size() < 4) {
			debugui->LogText(L"Use: randomterr cellX cellY subdelta");
			return;
		}
		CIrrStrParser pr2(list[1]);
		s32 clx = pr2.ToS32(); pr2 = list[2];
		s32 cly = pr2.ToS32(); pr2 = list[3];
		float subd = pr2.ToFloat();
		CGWIC_Cell* cptr = GetCell(clx,cly);
		if (cptr) {
			cptr->RandomizeTerrain(subd);
			debugui->LogText(L"Command done!");
		}
	} else {
		debugui->LogText(L"Invalid command!");
	}
}

void CGWIC_World::CmdGetPos(CIrrStrParser parse)
{
	irrstrwvec list = parse.ParseToList(L" ");
	if ((list.size() >= 2) && (list[1] == "selection")) {
		if (selected) {
			stringw vs;
			vector3df ps = selected->GetPos();
			vs += ps.X;
			vs += L" ";
			vs += ps.Y;
			vs += L" ";
			vs += ps.Z;
			vs += L" m";
			debugui->LogText(vs);
		} else
			debugui->LogText(L"Nothing selected");
	} else {
		//
	}
}

void CGWIC_World::TerrainMagnet()
{
	//testing terrain magnet mode
	//FIXME: discard unnecessary variables, this code produced from pure math ;)
	vector3df camvec = main_cam->getAbsolutePosition();
	float oh = GetTerrainHeightUnderPointMetric(camvec);
	float h = fabs(camvec.Y / GWIC_IRRUNITS_PER_METER);// + GWIC_IRRUNITS_PER_METER;
	const float b = 5.f * GWIC_IRRUNITS_PER_METER;
	const float dim = GWIC_METERS_PER_CELL * GWIC_IRRUNITS_PER_METER;
	const float step = dim / 256;
	float cx = camvec.X - b;
	float ny = camvec.Z - b;
	float bx = camvec.X + b;
	float by = camvec.Z + b;
	float cy;
	float ch;
	float dx;
	float dy;
	float dd;
	float ff;
	while (cx < bx) {
		cy = ny;
		dx = camvec.X - cx;
		while (cy < by) {
			dy = camvec.Z - cy;
			dd = floor(sqrt(dx*dx+dy*dy)/step);
			ff = dd / ceil(b/step);
//			oh = GetTerrainHeightUnderPointMetric(camvec);
			ch = ff*oh + (1.f-ff)*h;
			SetTerrainHeightUnderPointMetric(vector3df(cx,0,cy),ch,false);
			cy += step;
//			std::cout << "mag: " << cx << ':' << cy << '=' << ch << std::endl;
		}
		cx += step;
	}
	SetTerrainHeightUnderPointMetric(vector3df(cx-step,0,cy),ch,true);
}

void CGWIC_World::ReloadCell(CGWIC_Cell* cell)
{
	std::vector<CGWIC_Cell*>::iterator cit = cells.begin();
	for (; cit != cells.end(); ++cit)
		if (*cit == cell) break;
	if (cit == cells.end()) {
		std::cerr << "ReloadCell(): can't find cell by ptr!" << std::endl;
		return;
	}
	cit = cells.erase(cit);
	//FIXME: if RunWorld() is threaded, we need to block the threads until cell reloaded
	//FIXME: maybe implement operator '=' to reassign objects to the new cell
	CPoint2D crd = cell->GetCoord();
	delete cell;
	cell = new CGWIC_Cell(crd,gra_world,phy_world);
	if ((!cell) || (!cell->InitLand())) {
		std::cerr << "ReloadCell(): can't create new cell!" << std::endl;
		debugui->LogText("Cell reloading failed!");
	} else
		cells.insert(cit,cell);
}

void CGWIC_World::StitchTerrains(CGWIC_Cell* ca, CGWIC_Cell* cb, bool update)
{
	if ((!ca) || (!cb)) return;
	std::cout << "Stitch " << ca->GetCoord().X << ':' << ca->GetCoord().Y;
	std::cout << " with " << cb->GetCoord().X << ':' << cb->GetCoord().Y << std::endl;
	CPoint2D sidei = cb->GetCoord() - ca->GetCoord();
	vector3df sidef,shifta,shiftb;
	if (sidei.X) {
		sidef = vector3df(0,0,1);
		shifta = (sidei.X > 0)? vector3df(GWIC_METERS_PER_CELL,0,0):vector3df(0);
		shiftb = (sidei.X > 0)? vector3df(0):vector3df(GWIC_METERS_PER_CELL,0,0);
	} else if (sidei.Y) {
		sidef = vector3df(1,0,0);
		shifta = (sidei.Y > 0)? vector3df(0,0,GWIC_METERS_PER_CELL):vector3df(0);
		shiftb = (sidei.Y > 0)? vector3df(0):vector3df(0,0,GWIC_METERS_PER_CELL);
	}
	float ha,hb;
	for (u32 i=0; i<256; i++) {
		ha = ca->GetTerrainHeightUnderPointMetric(sidef*i+shifta);
		hb = cb->GetTerrainHeightUnderPointMetric(sidef*i+shiftb);
		ha = (ha+hb) / 2;
		ca->SetTerrainHeightUnderPointMetric(sidef*i+shifta,ha,((update)&&(i>254)));
		cb->SetTerrainHeightUnderPointMetric(sidef*i+shiftb,ha,((update)&&(i>254)));
	}
}

void CGWIC_World::StitchWorld(bool lite)
{
	CGWIC_Cell* curcentr;
	std::vector<CGWIC_Cell*> clst;
	CPoint2D pnt(0);
	s32 cxd = (properties.wrldSizeX % 2)? 0:1;
	u32 i;
	for (i=0; i<cells.size(); i++) cells[i]->SetActive(false);
	do {
		curcentr = GetCell(pnt);
		clst = GetNeighbors(pnt);
		while (clst.size()) {
			StitchTerrains(curcentr,clst.back(),(!lite));
			clst.pop_back();
		}
		pnt.X += 2;
		if (pnt.X >= properties.wrldSizeX) {
			pnt.X -= properties.wrldSizeX - cxd;
			pnt.Y++;
		}
	} while (pnt.Y < properties.wrldSizeY);
	ActivateCell(center_cell);
}

std::vector<CGWIC_Cell*> CGWIC_World::GetNeighbors(CPoint2D centr)
{
	std::vector<CGWIC_Cell*> out;
	CGWIC_Cell* ccell;
	for (u32 i=0; i<4; i++) {
		ccell = GetCell(centr+neighbor_array[i]);
		if (ccell) out.push_back(ccell);
	}
	return out;
}


} // namespace gwic
