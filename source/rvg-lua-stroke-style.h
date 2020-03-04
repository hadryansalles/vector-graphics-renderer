#ifndef RVG_LUA_STROKE_STYLE_H
#define RVG_LUA_STROKE_STYLE_H

#include "rvg-lua.h"

#include "rvg-stroke-style.h"

int rvg_lua_stroke_style_init(lua_State *L, int ctxidx);
int rvg_lua_stroke_style_export(lua_State *L, int ctxidx);
rvg::stroke_style::ptr rvg_lua_stroke_style_create(lua_State *L, int base);

#endif
