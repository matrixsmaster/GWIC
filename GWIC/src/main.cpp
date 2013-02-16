/*
 * main.cpp
 *  Created on: Dec 3, 2012
 *
 *	Gynoid World Interactive Constructor entry point file
 *	GWIC (C) Dmitry Soloviov, 2012-2013
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
	wrldSettings.viewDistance = 2500; //meters
	// HardCuller settings
	wrldSettings.hardcull.DistantLand = true;
	wrldSettings.hardcull.ActorsCullMeters = 70;
	wrldSettings.hardcull.ObjectCullMeters = 45;
}

//Note: This parser is quick & dirty, but it works :)
void arg_parser(int argc, char * argv[])
{
	char* key = NULL;
	int i,j;
	GWIC_ARGKEYTYPE typ;
	static char tbuf[4] = {0,0,0,0};
	size_t z;
	//don't init new vars inside cycles in this function!
	printf("\n[PARSE]: arg_parse:\targc=%d\n",argc);
	for (i=1; ((i<argc) && (argv[i]!=NULL)); i++) {
		printf("[PARSE]: Arg %04d:\t%s\n",i,argv[i]);
		if (key) {
			printf("[PARSE]: Key '%s' parameter: %s\n",key,argv[i]);
			for (j=0; j<GWIC_KEYS_ARR_SIZE; j++)
				if (!strcmp(key,GWIC_KEYS_ARRAY[j]))
					break;
			typ = static_cast<GWIC_ARGKEYTYPE> (j); //next after last is unknown, so its all good ;)
			switch (typ) {
			case GWIC_AKEY_VFSROOT:
				printf("root\n");
				wrldSettings.vfsRoot = "";
				wrldSettings.vfsRoot += argv[i];
				break;
			case GWIC_AKEY_VFSTYPE:
				printf("type\n");
				z = strlen(argv[i]);
				memcpy(tbuf,argv[i],((z>4)?4:z));
				wrldSettings.vfsType = static_cast<irr::io::E_FILE_ARCHIVE_TYPE>
						(MAKE_IRR_ID(tbuf[0],tbuf[1],tbuf[2],tbuf[3]));
				break;
			case GWIC_AKEY_UNKNOWN:
			default:
				printf("[PARSE]: Unknown key '%s'! Parameter bypassed.\n",key);
			}
			key = NULL;
		} else {
			if (argv[i][0] == '-') {
				key = argv[i];
				printf("[PARSE]: key: %s\n",key);
			}
		}
	}
	printf("\n[PARSE]: Parse done!\n");
}

int main(int argc, char * argv[])
{
	printf("\nGynoid World Interactive Constructor\n");
	printf("%s\n",GWIC_CONSOLE_VERSIONSTR);
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
