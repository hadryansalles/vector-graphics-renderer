#ifndef RVG_LUA_RECT_DATA_H
#define RVG_LUA_RECT_DATA_H

#include "rvg-lua.h"

#include "rvg-rect-data.h"

int rvg_lua_rect_data_init(lua_State *L, int ctxidx);
int rvg_lua_rect_data_export(lua_State *L, int ctxidx);
rvg::rect_data::ptr rvg_lua_rect_data_create(lua_State *L, int base);

#endif
