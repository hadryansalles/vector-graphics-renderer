#include <cmath>

#include "rvg-lua.h"
#include "rvg-lua-path-filters.h"
#include "rvg-lua-scene-filters.h"

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_filter(lua_State *L) {
    rvg_lua_init(L); // ctxtab
    int ctxidx = lua_gettop(L);
    lua_newtable(L); // ctxtab mod
    rvg_lua_path_filters_export(L, ctxidx);
    rvg_lua_scene_filters_export(L, ctxidx);
    return 1;
}
