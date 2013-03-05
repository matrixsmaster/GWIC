/*
 * CGWIC_CommandPU.h
 *
 *  Created on: Jan 29, 2013
 *
 *  Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_COMMANDPU_H_
#define CGWIC_COMMANDPU_H_

#include "GWICDefines.h"
#include "GWICTypes.h"

namespace gwic {

enum GWIC_CMDPU_CMD {
	GCMD_quit = 0,
	GCMD_test,
	GCMD_help,
	GCMD_randomplace,
	GCMD_tpm,
	GCMD_tdd,
	GCMD_reloadcell,
	GCMD_stitch,
	GCMD_restitch,
	GCMD_createwindow,
	GCMD_randomterr,
	GCMD_attachplayername,
	GCMD_annulateterrachanges,
	GCMD_setdistantland,
	GCMD_flushcell,
	GCMD_getobjcount,
	GCMD_placeatcam,
	GCMD_listactornames,
	GCMD_lightatcam_test,
	GCMD_sunatcam,
	GCMD_UNKNOWN
};

struct GWIC_CMDPU_tabdata {
	GWIC_CMDPU_CMD cmdtype;
	std::string cmdstring;
	//TODO: add short help strings to commands
//	std::string cmdshorthelp;
};

static const GWIC_CMDPU_tabdata commandPU_commandlist[] = {
		{GCMD_quit,"quit"},
		{GCMD_test,"test"},
		{GCMD_help,"help"},
		{GCMD_randomplace,"randomplace"},
		{GCMD_tpm,"tpm"},
		{GCMD_tdd,"tdd"},
		{GCMD_reloadcell,"reloadcell"},
		{GCMD_stitch,"stitch"},
		{GCMD_restitch,"restitch"},
		{GCMD_createwindow,"createwindow"},
		{GCMD_randomterr,"randomterr"},
		{GCMD_attachplayername,"attachplayername"},
		{GCMD_annulateterrachanges,"annulateterrachanges"},
		{GCMD_setdistantland,"setdistantland"},
		{GCMD_flushcell,"flushcell"},
		{GCMD_getobjcount,"getobjcount"},
		{GCMD_placeatcam,"placeatcam"},
		{GCMD_listactornames,"listactornames"},
		{GCMD_lightatcam_test,"lightatcam_test"},
		{GCMD_sunatcam,"sunatcam"},
		{GCMD_UNKNOWN,""}
};

//FIXME: Add standard Message Broadcasting System
//look at GWICTypes
struct GWIC_CMDPU_cmdata {
	CIrrStrParser parse;
	irr::core::stringw receiver;
};

class CGWIC_CommandPU {
public:
	CGWIC_CommandPU();
	virtual ~CGWIC_CommandPU();
	void SetWorld(void * wrld_ptr);
	void Process(irr::core::stringw cmd);
	irr::core::stringw GetNextOutput();
protected:
	void Store(irr::core::stringw rec, irr::core::stringw str);
	void Error(irr::core::stringw rec, irr::core::stringw str);
private:
	struct GCMDPU_hData;
	GCMDPU_hData* hData;
	/* Commands */
	void cmd_test(GWIC_CMDPU_cmdata *input);
	void cmd_randomplace(GWIC_CMDPU_cmdata *input);
	void cmd_reloadcell(GWIC_CMDPU_cmdata *input);
	void cmd_stitch(GWIC_CMDPU_cmdata *input);
	void cmd_createwindow(GWIC_CMDPU_cmdata *input);
	void cmd_randomterr(GWIC_CMDPU_cmdata *input);
	void cmd_attachplayername(GWIC_CMDPU_cmdata *input);
	void cmd_setdistantland(GWIC_CMDPU_cmdata *input);
	void cmd_flushcell(GWIC_CMDPU_cmdata *input);
	void cmd_getobjcount(GWIC_CMDPU_cmdata *input);
	void cmd_placeatcam(GWIC_CMDPU_cmdata *input);
	void cmd_listactornames(GWIC_CMDPU_cmdata *input);
	void cmd_lightatcam_test(GWIC_CMDPU_cmdata *input);
	void cmd_sunatcam(GWIC_CMDPU_cmdata *input);
};

} /* namespace gwic */
#endif /* CGWIC_COMMANDPU_H_ */
