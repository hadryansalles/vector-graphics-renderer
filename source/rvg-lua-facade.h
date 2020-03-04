#ifndef RVG_LUA_FACADE_DRIVER_H
#define RVG_LUA_FACADE_DRIVER_H

#include "rvg-lua.h"

#include "rvg-scene-data.h"

int rvg_lua_facade_init(lua_State *L, int ctxidx);
rvg::scene_data::ptr rvg_lua_facade_scene_data_create(lua_State *L, int base);
int rvg_lua_facade_new_driver(lua_State *L, const luaL_Reg *funcs);

#endif
