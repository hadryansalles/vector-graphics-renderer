#ifndef RVG_LUA_SCENE_H
#define RVG_LUA_SCENE_H

#include "rvg-lua.h"

#include "rvg-scene.h"

int rvg_lua_scene_init(lua_State *L, int ctxidx);
int rvg_lua_scene_export(lua_State *L, int ctxidx);
rvg::scene rvg_lua_scene_create(lua_State *L, int base);

#endif
