#include <cstdio>
#include <sstream>

#include "rvg-lua.h"
#include "rvg-base64.h"

static int encode(lua_State *L) {
    size_t len = 0;
    const char *str = luaL_checklstring(L, 1, &len);
    std::string encoded = rvg::encode_base64(std::string(str, len));
    lua_pushlstring(L, encoded.data(), encoded.length());
    return 1;
}

static int decode(lua_State *L) {
    size_t len = 0;
    const char *str = luaL_checklstring(L, 1, &len);
    std::string decoded = rvg::decode_base64(std::string(str, len));
    lua_pushlstring(L, decoded.data(), decoded.length());
    return 1;
}

static const luaL_Reg mod[] = {
    {"encode", encode},
    {"decode", decode},
    {NULL, NULL}
};

extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_base64(lua_State *L) {
    lua_newtable(L);
    rvg_lua_setfuncs(L, mod, 0);
    return 1;
}
