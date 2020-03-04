#ifndef RVG_LUA_CHRONOS_H
#define RVG_LUA_CHRONOS_H

#include "rvg-lua.h"

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_chronos(lua_State *L);

#endif // RVG_LUA_CHRONOS_H
