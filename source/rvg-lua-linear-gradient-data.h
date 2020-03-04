#ifndef RVG_LUA_LINEAR_GRADIENT_DATA_H
#define RVG_LUA_LINEAR_GRADIENT_DATA_H

#include "rvg-lua.h"

#include "rvg-linear-gradient-data.h"

int rvg_lua_linear_gradient_data_init(lua_State *L, int ctxidx);
int rvg_lua_linear_gradient_data_export(lua_State *L, int ctxidx);
rvg::linear_gradient_data::ptr rvg_lua_linear_gradient_data_create(lua_State *L, int base);

#endif
