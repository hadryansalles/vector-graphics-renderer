#ifndef RVG_LUA_TRIANGLE_DATA_H
#define RVG_LUA_TRIANGLE_DATA_H

#include "rvg-lua.h"

#include "rvg-triangle-data.h"

int rvg_lua_triangle_data_init(lua_State *L, int ctxidx);
int rvg_lua_triangle_data_export(lua_State *L, int ctxidx);
rvg::triangle_data::ptr rvg_lua_triangle_data_create(lua_State *L, int base);

#endif
