#ifndef RVG_LUA_TEXTURE_DATA_H
#define RVG_LUA_TEXTURE_DATA_H

#include "rvg-lua.h"

#include "rvg-texture-data.h"

int rvg_lua_texture_data_init(lua_State *L, int ctxidx);
int rvg_lua_texture_data_export(lua_State *L, int ctxidx);
rvg::texture_data::ptr rvg_lua_texture_data_create(lua_State *L, int base);

#endif
