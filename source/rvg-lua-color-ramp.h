#ifndef RVG_LUA_COLOR_RAMP_H
#define RVG_LUA_COLOR_RAMP_H

#include "rvg-lua.h"

#include "rvg-color-ramp.h"

int rvg_lua_color_ramp_init(lua_State *L, int ctxidx);
int rvg_lua_color_ramp_export(lua_State *L, int ctxidx);
rvg::color_ramp::const_ptr rvg_lua_color_ramp_create(lua_State *L, int base);

#endif
