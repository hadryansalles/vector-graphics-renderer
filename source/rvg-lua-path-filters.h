#ifndef RVG_LUA_PATH_FILTER_H
#define RVG_LUA_PATH_FILTER_H

#include "rvg-lua.h"

int rvg_lua_path_filters_init(lua_State *L, int ctxidx);
int rvg_lua_path_filters_export(lua_State *L, int ctxidx);

#endif
