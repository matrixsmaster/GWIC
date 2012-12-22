/*
 * CGWIC_World.cpp
 *
 *  Created on: Dec 3, 2012
 *      Author: Soloviov Dmitry
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
	switch(event.EventType)
	{
	case EET_MOUSE_INPUT_EVENT:
		if(event.MouseInput.Event==EMIE_LMOUSE_PRESSED_DOWN)
		{
			//
			return false;
		} else if(event.MouseInput.Event==EMIE_RMOUSE_PRESSED_DOWN) {
			//
			return false;
		}
		mousepos.X = event.MouseInput.X;
		mousepos.Y = event.MouseInput.Y;
		mousewheel = event.MouseInput.Wheel;
		break;
	case EET_KEY_INPUT_EVENT:
		//FIXME: more robust and nice looking processing needed!!
		if(event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown == false) {
//			fps_cam ^= true;
			if (!fps_cam) GoFPS();
			else GoEditMode();
			return true;
		}
		if (!fps_cam) break;
		if(event.KeyInput.Key == KEY_KEY_Q && event.KeyInput.PressedDown == false) {
			quit_msg = true;
			return true;
		}
		if(event.KeyInput.Key == KEY_SPACE && event.KeyInput.PressedDown == false) {
			if (fps_cam) {
				float ssize = GWIC_IRRUNITS_PER_METER / 4.f;
				this->ShootSphere(vector3df(ssize,ssize,ssize),3.0);
			}
			return true;
		}
		if(event.KeyInput.Key == KEY_DELETE && event.KeyInput.PressedDown == false) {
			std::cout << "Deleting main camera!" << std::endl;
			main_cam->remove();
			main_cam = NULL;
			return true;
		}
		if(event.KeyInput.Key == KEY_F9 && event.KeyInput.PressedDown == false) {
			physicsPause ^= true;
			if (physicsPause) debugui->LogText(L"Physics paused");
			else debugui->LogText(L"Physics resumed");
			return true;
		}
		break;
	default: break;
	}
	//pump messages
	if (event.EventType == EET_GUI_EVENT) {
		debugui->PumpMessage(event);
		for (u32 i=0; i<uis.size(); i++)
			uis[i]->PumpMessage(event);
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
			std::cout << "Cell search complete in first phase!" << std::endl;
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
	//cells = reinterpret_cast<CGWIC_Cell*> (malloc(sizeof(CGWIC_Cell*) * properties.wrldSizeX * properties.wrldSizeY));
	int y;
	for (int x=0; x<properties.wrldSizeX; x++)
		for (y=0; y<properties.wrldSizeY; y++) {
			ptr = new CGWIC_Cell(CPoint2D(x,y),gra_world,phy_world);
			ptr->InitLand();
			cells.push_back(ptr);
		}
	return true;
}

bool CGWIC_World::GenerateNPC()
{
	BotCreationParams botset;
	botset.type = ACTOR_DUMMY;
	botset.cell_coord = CPoint2D(2);
	botset.rel_pos = vector3df(120,50,120);
	botset.height = 1.5f;
	actors.push_back(new CGWIC_Bot(&botset,gra_world,phy_world));
	actors.back()->SetEnabled(true);
	botset.type = ACTOR_NPC;
	botset.rel_pos = vector3df(110,50,110);
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
	std::cout << "Creating debug UI" << std::endl;
	debugui = new CGWIC_DebugUI(gra_world);
	if (!debugui) return false;
	debugui->SetPos(CPoint2D(150,16));
	debugui->Update();

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
	ISceneCollisionManager* pmgr = scManager->getSceneCollisionManager();
	int lFPS = -1;
	int cFPS;
	ticker = 0;
	line3d<f32> ray;
	vector3df rayhit;
	triangle3df hit_triag;
	ISceneNode* selected;
	stringw cmdstr;
	while (gra_world->run()) {
		ticker++;
		if (!gra_world->isWindowActive()) {
			gra_world->yield();
			continue;
		}
		ProcessEvents();
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

		if (main_cam) {
			selected = NULL;
			if (fps_cam) {
				ray.start = main_cam->getPosition();
				ray.end = ray.start + (main_cam->getTarget() - ray.start).normalize() * main_cam->getFarValue();
			} else {
				ray = pmgr->getRayFromScreenCoordinates(vector2di(mousepos.X,mousepos.Y),main_cam);
			}
			selected = pmgr->getSceneNodeAndCollisionPointFromRay(ray,rayhit,hit_triag,GWIC_PICKABLE_MASK,0);
			//TODO: get selected object via cell's GetObjecyByIrrPtr()
			if (selected) {
	//			std::cerr << selected->getPosition().Y << std::endl;
				if (selpoint_bill) selpoint_bill->setPosition(rayhit);
	//			std::cerr << rayhit.Y << std::endl;
			} else {
				if (selpoint_bill) selpoint_bill->setPosition(vector3df(0));
			}
		}

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

		//FIXME: need more robust update scheduling (on time, and in separate thread)
		if (ticker > 25) {
			ticker = 0;
			for (u32 i=0; i<cells.size(); i++)
				cells[i]->Update();
			CellTransfers(); //FIXME: separate thread! (sync to cell updates certainly)
		}
		//process commands (if runworld() gets threaded, this need to be sync to graphic and physics)
		if (ticker%2) {
			while (!(cmdstr = debugui->GetNextCommand()).empty())
				CommandProcessor(cmdstr);
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
				cptr = GetCell(targ.X,targ.Y);
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
		CGWIC_Cell* ptr = GetCell(center_cell.X,center_cell.Y);
		oldpos = ptr->getIrrlichtCenter();
	}
	std::cout << "Camera set to First-Person mode" << std::endl;
	if (debugui) debugui->LogText(L"Camera set to First-Person mode");
	main_cam = scManager->addCameraSceneNodeFPS();
	main_cam->setPosition(oldpos);
	main_cam->setFarValue(properties.viewDistance * GWIC_IRRUNITS_PER_METER);
	ShowGUI(false);
	fps_cam = true;
}

void CGWIC_World::GoEditMode()
{
	vector3df oldpos(0);
	if (main_cam) {
		main_cam->remove();
		oldpos = (fps_cam)? main_cam->getPosition():main_cam->getTarget();
	} else {
		CGWIC_Cell* ptr = GetCell(center_cell.X,center_cell.Y);
		oldpos = ptr->getIrrlichtCenter();
	}
	std::cout << "Camera set to Maya mode" << std::endl;
	if (debugui) debugui->LogText(L"Camera set to Maya mode");
	main_cam = scManager->addCameraSceneNodeMaya();
	main_cam->setTarget(oldpos);
	if (selpoint_bill) {
		selpoint_bill->remove();
		selpoint_bill = NULL;
	}
	main_cam->setFarValue(properties.viewDistance * GWIC_IRRUNITS_PER_METER);
	ShowGUI(true);
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

float CGWIC_World::GetTerrainHeightUnderPoint(irr::core::vector3df pnt)
{
	//TODO: find cell and use cell's terrain height func
	return 0;
}

void CGWIC_World::ProcessEvents()
{
	//TODO: are we really wanna it? :)
}

void CGWIC_World::CommandProcessor(irr::core::stringw cmd)
{
	stringc cons = cmd.c_str();
	std::cout << "Command processor: cmd: " << cons.c_str() << std::endl;
	if (cmd == "quit") {
		quit_msg = true;
	} else if (cmd == "test") {
		debugui->LogText("Test Response String");
	}
}


}
