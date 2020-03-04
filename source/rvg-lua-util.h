#ifndef RVG_LUA_UTIL_H
#define RVG_LUA_UTIL_H

#include "rvg-lua.h"

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_util(lua_State *L);

#endif // RVG_LUA_UTIL_H
