#ifndef RVG_LUA_BBOX_H
#define RVG_LUA_BBOX_H

#include "rvg-lua.h"

#include "rvg-window.h"
#include "rvg-viewport.h"

int rvg_lua_bbox_init(lua_State *L, int ctxidx);
int rvg_lua_bbox_export(lua_State *L, int ctxidx);
rvg::window rvg_lua_window_create(lua_State *L, int base);
rvg::viewport rvg_lua_viewport_create(lua_State *L, int base);

#endif
