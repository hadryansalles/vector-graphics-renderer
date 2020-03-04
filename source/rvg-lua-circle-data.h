#ifndef RVG_LUA_CIRCLE_DATA_H
#define RVG_LUA_CIRCLE_DATA_H

#include "rvg-lua.h"

#include "rvg-circle-data.h"

int rvg_lua_circle_data_init(lua_State *L, int ctxidx);
int rvg_lua_circle_data_export(lua_State *L, int ctxidx);
rvg::circle_data::ptr rvg_lua_circle_data_create(lua_State *L, int base);

#endif
