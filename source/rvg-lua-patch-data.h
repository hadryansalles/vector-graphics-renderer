#ifndef RVG_LUA_PATCH_DATA_H
#define RVG_LUA_PATCH_DATA_H

#include "rvg-patch-data.h"

int rvg_lua_patch_data_init(lua_State *L, int ctxidx);
int rvg_lua_patch_data_export(lua_State *L, int ctxidx);

template <typename PATCH_DATA>
typename PATCH_DATA::const_ptr
rvg_lua_patch_data_create(lua_State *L, int base);

extern template
rvg::patch_data<16,4>::const_ptr
rvg_lua_patch_data_create<rvg::patch_data<16,4>>(lua_State *L, int base);

extern template
rvg::patch_data<12,4>::const_ptr
rvg_lua_patch_data_create<rvg::patch_data<12,4>>(lua_State *L, int base);

extern template
rvg::patch_data<3,3>::const_ptr
rvg_lua_patch_data_create<rvg::patch_data<3,3>>(lua_State *L, int base);

#endif
