#ifndef RVG_LUA_WINDING_RULE_H
#define RVG_LUA_WINDING_RULE_H

#include "rvg-lua.h"

#include "rvg-winding-rule.h"

int rvg_lua_winding_rule_init(lua_State *L, int ctxidx);
int rvg_lua_winding_rule_export(lua_State *L, int ctxidx);

#endif
