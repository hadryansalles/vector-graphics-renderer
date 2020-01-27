#include "rvg-lua.h"
#include "rvg-lua-spread.h"

using namespace rvg;

static rvg_lua_named_value<e_spread> named_spreads[] = {
    {"clamp", e_spread::clamp},
    {"mirror", e_spread::mirror},
    {"wrap", e_spread::wrap},
    {"transparent", e_spread::transparent},
    { nullptr, e_spread::transparent},
};

int rvg_lua_spread_init(lua_State *L, int ctxidx) {
    if (!rvg_lua_typeexists<e_spread>(L, ctxidx)) {
        rvg_lua_enum_createtype<e_spread>(L, "spread",
            named_spreads, ctxidx);
    }
    return 0;
}

int rvg_lua_spread_export(lua_State *L, int ctxidx) {
    // mod
    rvg_lua_spread_init(L, ctxidx); // mod
    rvg_lua_enum_pushmap<e_spread>(L, ctxidx); // mod map
    rvg_lua_readonlyproxy(L); // mod mapproxy
    lua_setfield(L, -2, "spread"); // mod
    return 0;
}
