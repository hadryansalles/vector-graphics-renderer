#ifndef RVG_LUA_HARFBUZZ_H
#define RVG_LUA_HARFBUZZ_H

#include "rvg-lua.h"

int rvg_lua_harfbuzz_export(lua_State *L, int ctxidx);
int rvg_lua_harfbuzz_init(lua_State *L, int ctxidx);

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_harfbuzz(lua_State *L);

#endif // RVG_LUA_HARFBUZZ_H
