#include "rvg-lua.h"

#include "rvg-lua-winding-rule.h"

using namespace rvg;

static rvg_lua_named_value<e_winding_rule> named_winding_rules[] = {
    {"non_zero", e_winding_rule::non_zero},
    {"odd", e_winding_rule::odd},
    {"zero", e_winding_rule::zero},
    {"even", e_winding_rule::even},
    {nullptr, e_winding_rule::even},
};

int rvg_lua_winding_rule_init(lua_State *L, int ctxidx) {
    if (!rvg_lua_typeexists<e_winding_rule>(L, ctxidx)) {
        rvg_lua_enum_createtype<e_winding_rule>(L, "winding_rule",
            named_winding_rules, ctxidx);
    }
    return 0;
}

int rvg_lua_winding_rule_export(lua_State *L, int ctxidx) {
    // mod
    rvg_lua_winding_rule_init(L, ctxidx); // mod
    rvg_lua_enum_pushmap<e_winding_rule>(L, ctxidx); // mod map
    rvg_lua_readonlyproxy(L); // mod mapproxy
    lua_setfield(L, -2, "winding_rule"); // mod
    return 0;
}
