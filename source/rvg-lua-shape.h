#ifndef RVG_LUA_SHAPE_H
#define RVG_LUA_SHAPE_H

#include "rvg-lua.h"

#include "rvg-shape.h"

int rvg_lua_shape_init(lua_State *L, int ctxidx);
int rvg_lua_shape_export(lua_State *L, int ctxidx);

#endif
