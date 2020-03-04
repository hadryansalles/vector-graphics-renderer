#ifndef RVG_LUA_SPREAD_H
#define RVG_LUA_SPREAD_H

#include "rvg-lua.h"

#include "rvg-spread.h"

int rvg_lua_spread_init(lua_State *L, int ctxidx);
int rvg_lua_spread_export(lua_State *L, int ctxidx);

#endif
