#ifndef RVG_LUA_PATH_DATA_H
#define RVG_LUA_PATH_DATA_H

#include "rvg-lua.h"

#include "rvg-path-data.h"

int rvg_lua_path_data_init(lua_State *L, int ctxidx);
int rvg_lua_path_data_export(lua_State *L, int ctxidx);
rvg::path_data::ptr rvg_lua_path_data_create(lua_State *L, int base);

#endif
