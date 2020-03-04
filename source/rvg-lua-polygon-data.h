#ifndef RVG_LUA_POLYGON_DATA_H
#define RVG_LUA_POLYGON_DATA_H

#include "rvg-lua.h"

#include "rvg-polygon-data.h"

int rvg_lua_polygon_data_init(lua_State *L, int ctxidx);
int rvg_lua_polygon_data_export(lua_State *L, int ctxidx);
rvg::polygon_data::ptr rvg_lua_polygon_data_create(lua_State *L, int base);

#endif
