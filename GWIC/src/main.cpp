/*
 * main.cpp
 *  Created on: Dec 3, 2012
 *
 *	Gynoid World Interactive Constructor entry point file
 *	GWIC (C) Dmitry Soloviov, 2012
 *	Licensed under the terms of GNU GPL v2
 *	use can see LICENSE for details
 */

#include "main.h"

using namespace gwic;

#ifdef GWIC_USES_OPENAL
int init_openal(void)
{
	printf("Initializing OpenAL... ");
	OAL_SetupLogging(true,eOAL_LogOutput_Console,eOAL_LogVerbose_High);
	cOAL_Init_Params oal_parms;
	oal_parms.mlStreamingBufferSize = 8192;
	if (OAL_Init(oal_parms) == false) {
		printf("Failed - Check your OpenAL installation\n");
		return 1;
	} else
		printf("Success\n");
	OAL_Source_Stop(OAL_ALL);
	return 0;
}
#endif

void print_help(void)
{
	printf("Some help here :)\n");
}

void param_default(void)
{
	// renderer settings
	wrldSettings.bits = 32;
	wrldSettings.gWidth = 1024;
	wrldSettings.gHeight = 768;
	wrldSettings.fullscreen = false;
	wrldSettings.stencil = false;
	wrldSettings.vsync = false;
	wrldSettings.videoDriver = irr::video::EDT_OPENGL;
	wrldSettings.vfsRoot = GWIC_FILESYSTEMROOT;
	wrldSettings.vfsType = GWIC_ROOTFS_TYPE;
	// World settings
	wrldSettings.wrldSizeX = 3;
	wrldSettings.wrldSizeY = 3;
	wrldSettings.startX = 2;
	wrldSettings.startY = 2;
	wrldSettings.minLevel = -10.f * GWIC_IRRUNITS_PER_METER;
	wrldSettings.waterLevel = -10.f * GWIC_IRRUNITS_PER_METER;
	// physics
	wrldSettings.Gforce = -9.8f * GWIC_IRRUNITS_PER_METER;
	wrldSettings.debugdraw = true;
	// camera
	wrldSettings.viewDistance = 5000; //meters
}

void arg_parser(int argc, char * argv[])
{
	//TODO: add argument parsing!
	printf("\narg_parse:\nargc=%d\n",argc);
	char *ptr = argv[0];
	for (int i=0; ((i<argc) && (ptr!=NULL)); i++) {
		printf("Arg %04d:\t%s\n",i,argv[i]);
	}
}

int main(int argc, char * argv[])
{
	printf("\nGynoid World Interactive Constructor\n");
	srand((unsigned)time(0));
#ifdef GWIC_USES_OPENAL
	if (init_openal()) return 0;
#endif
	param_default();
	arg_parser(argc,argv);
	ourworld = new CGWIC_World(&wrldSettings,gpDevice);
	if (ourworld->PrepareWorld()) {
		printf("World created!\n");
		ourworld->RunWorld();
	} else
		printf("World creation failed!\n");
	delete ourworld;
#ifdef GWIC_USES_OPENAL
	OAL_Close();
#endif
	return 0;
}
