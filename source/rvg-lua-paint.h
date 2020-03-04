#ifndef RVG_LUA_PAINT_H
#define RVG_LUA_PAINT_H

#include "rvg-lua.h"

#include "rvg-paint.h"

int rvg_lua_paint_init(lua_State *L, int ctxidx);
int rvg_lua_paint_export(lua_State *L, int ctxidx);

#endif
