#ifndef RVG_LUA_IMAGE_H
#define RVG_LUA_IMAGE_H

#include "rvg-lua.h"

#include "rvg-image.h"

int rvg_lua_image_init(lua_State *L, int ctxidx);
int rvg_lua_image_export(lua_State *L, int ctxidx);
rvg::i_image::ptr rvg_lua_image_create(lua_State *L, int base);

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_image(lua_State *L);

#endif // RVG_LUA_IMAGE_H
