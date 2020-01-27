#ifndef RVG_LUA_XFORM_H
#define RVG_LUA_XFORM_H

#include <lua.h>

#include "rvg-xform.h"

int rvg_lua_xform_init(lua_State *L, int ctxidx);
int rvg_lua_xform_export(lua_State *L, int ctxidx);
rvg::xform rvg_lua_xform_identity(lua_State *L, int base);
rvg::xform rvg_lua_xform_rotation(lua_State *L, int base);
rvg::xform rvg_lua_xform_scaling(lua_State *L, int base);
rvg::xform rvg_lua_xform_translation(lua_State *L, int base);
rvg::xform rvg_lua_xform_linear(lua_State *L, int base);
rvg::xform rvg_lua_xform_affinity(lua_State *L, int base);
rvg::xform rvg_lua_xform_projectivity(lua_State *L, int base);
rvg::xform rvg_lua_xform_windowviewport(lua_State *L, int base);

#endif
