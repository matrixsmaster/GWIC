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

class CGWIC_CommandPU {
public:
	CGWIC_CommandPU();
	virtual ~CGWIC_CommandPU();
	void SetWorld(void * wrld_ptr);
	void Process(irr::core::stringw cmd);
	irr::core::stringw GetNextOutput();
protected:
	void Store(irr::core::stringw rec, irr::core::stringw str);
private:
	struct GCMDPU_hData;
	GCMDPU_hData* hData;
};

} /* namespace gwic */
#endif /* CGWIC_COMMANDPU_H_ */
