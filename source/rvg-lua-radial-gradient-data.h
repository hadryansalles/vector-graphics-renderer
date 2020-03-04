#ifndef RVG_LUA_RADIAL_GRADIENT_DATA_H
#define RVG_LUA_RADIAL_GRADIENT_DATA_H

#include "rvg-lua.h"

#include "rvg-radial-gradient-data.h"

int rvg_lua_radial_gradient_data_init(lua_State *L, int ctxidx);
int rvg_lua_radial_gradient_data_export(lua_State *L, int ctxidx);
rvg::radial_gradient_data::ptr rvg_lua_radial_gradient_data_create(lua_State *L, int base);

#endif
