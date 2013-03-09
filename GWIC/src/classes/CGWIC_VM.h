/*
 * CGWIC_VM.h
 *  Created on: Dec 22, 2012
 *
 *	Authors:
 *	Soloviov Dmitry aka matrixsmaster
 */

#ifndef CGWIC_VM_H_
#define CGWIC_VM_H_

#include <GWICTypes.h>
#include <lua.hpp>
#include <irrlicht.h>

namespace gwic {

class CGWIC_VM {
public:
	CGWIC_VM(void *worldptr);
	virtual ~CGWIC_VM();
	bool LoadScriptFromLuaFile(irr::io::path filename);
	bool LoadScriptFromString(const char *str);
	GWICVM_VMState GetState() { return curstate; }
protected:
	struct GWICVM_hData;
	GWICVM_hData *hData;
	lua_State *luavm;
	GWICVM_VMState curstate;
private:
	//
};

} /* namespace gwic */
#endif /* CGWIC_VM_H_ */
