#include "rvg-lua.h"
#include "rvg-lua-xform.h"
#include "rvg-lua-xformable.h"
#include "rvg-lua-scene-data.h"

#include "rvg-lua-scene.h"
#include "rvg-lua-facade.h"

using namespace rvg;

static int scene_get_scene_data(lua_State *L) {
	const auto &c = rvg_lua_check<scene>(L, 1);
	rvg_lua_push<scene_data::const_ptr>(L,
		c.get_scene_data_ptr());
    return 1;
}

static luaL_Reg scene__index[] = {
    {"get_scene_data", scene_get_scene_data },
    { nullptr, nullptr }
};

scene rvg_lua_scene_create(lua_State *L, int base) {
    return scene{rvg_lua_facade_scene_data_create(L, base)};
}

static int create(lua_State *L) {
    return rvg_lua_push<scene>(L, rvg_lua_scene_create(L, 1));
}

static const luaL_Reg mod_scene[] = {
    {"scene", create},
    {NULL, NULL}
};

int rvg_lua_scene_init(lua_State *L, int ctxidx) {
    rvg_lua_xform_init(L, ctxidx);
    rvg_lua_scene_data_init(L, ctxidx);
    if (!rvg_lua_typeexists<scene>(L, ctxidx)) {
		rvg_lua_createtype<scene>(L, "scene", ctxidx);
		rvg_lua_set_xformable<scene>(L, ctxidx);
		rvg_lua_setmethods<scene>(L, scene__index, 0, ctxidx);
    }
    return 0;
}

int rvg_lua_scene_export(lua_State *L, int ctxidx) {
    // mod
    rvg_lua_scene_init(L, ctxidx); // mod
    lua_pushvalue(L, ctxidx); // mod ctxtab
    rvg_lua_setfuncs(L, mod_scene, 1); // mod
    return 0;
}
