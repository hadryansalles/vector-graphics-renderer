#include <cassert>
#include <ostream>

#include "rvg-lua.h"
#include "rvg-lua-xform.h"
#include "rvg-lua-xformable.h"
#include "rvg-lua-bbox.h"

using namespace rvg;

template <>
int rvg_lua_tostring<xform>(lua_State *L) {
    const auto &xf = *reinterpret_cast<xform *>(lua_touserdata(L, 1));
    lua_pushfstring(L, "xform{%f,%f,%f,%f,%f,%f}", xf[0][0], xf[0][1], xf[0][2],
       xf[1][0], xf[1][1], xf[1][2]);
    return 1;
}

rvg::xform rvg_lua_xform_identity(lua_State *L, int base) {
	if (lua_gettop(L) > base) luaL_error(L, "invalid number of arguments");
	return make_identity();
}

rvg::xform rvg_lua_xform_rotation(lua_State *L, int base) {
    base = base-1;
	switch (lua_gettop(L)-base) {
		case 1:
			return make_rotation(static_cast<rvgf>(luaL_checknumber(L, 1+base)));
		case 3:
			return make_rotation(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
                static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)));
		default:
			luaL_error(L, "invalid number of arguments");
            return make_identity();
	}
}

rvg::xform rvg_lua_xform_scaling(lua_State *L, int base) {
    base = base-1;
	switch (lua_gettop(L)-base) {
		case 1:
			return make_scaling(static_cast<rvgf>(luaL_checknumber(L, 1+base)));
		case 2:
			return make_scaling(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
                static_cast<rvgf>(luaL_checknumber(L, 2+base)));
		case 3:
			return make_scaling(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
                static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)));
		case 4:
			return make_scaling(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
                static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)),
                static_cast<rvgf>(luaL_checknumber(L, 4+base)));
		default:
			luaL_error(L, "invalid number of arguments");
            return make_identity();
	}
}

rvg::xform rvg_lua_xform_translation(lua_State *L, int base) {
    base = base-1;
    if (lua_gettop(L)-base > 2) luaL_error(L, "invalid number of arguments");
    return make_translation(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
        static_cast<rvgf>(luaL_optnumber(L, 2+base, 0.f)));
}

rvg::xform rvg_lua_xform_linear(lua_State *L, int base) {
    base = base-1;
    if (lua_gettop(L)-base != 4)
		luaL_error(L, "invalid number of arguments");
    return make_linear(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
        static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)),
        static_cast<rvgf>(luaL_checknumber(L, 4+base)));
}

rvg::xform rvg_lua_xform_affinity(lua_State *L, int base) {
    base = base-1;
    if (lua_gettop(L)-base != 6)
		luaL_error(L, "invalid number of arguments");
    return make_affinity(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
        static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)),
        static_cast<rvgf>(luaL_checknumber(L, 4+base)), static_cast<rvgf>(luaL_checknumber(L, 5+base)),
        static_cast<rvgf>(luaL_checknumber(L, 6+base)));
}

rvg::xform rvg_lua_xform_projectivity(lua_State *L, int base) {
    base = base-1;
    if (lua_gettop(L)-base != 9)
		luaL_error(L, "invalid number of arguments");
    return make_projectivity(static_cast<rvgf>(luaL_checknumber(L, 1+base)),
        static_cast<rvgf>(luaL_checknumber(L, 2+base)), static_cast<rvgf>(luaL_checknumber(L, 3+base)),
        static_cast<rvgf>(luaL_checknumber(L, 4+base)), static_cast<rvgf>(luaL_checknumber(L, 5+base)),
        static_cast<rvgf>(luaL_checknumber(L, 6+base)), static_cast<rvgf>(luaL_checknumber(L, 7+base)),
        static_cast<rvgf>(luaL_checknumber(L, 8+base)), static_cast<rvgf>(luaL_checknumber(L, 9+base)));
}

rvg::xform rvg_lua_xform_windowviewport(lua_State *L, int base) {
    base = base-1;
    if (lua_gettop(L)-base != 2)
		luaL_error(L, "invalid number of arguments");
    return make_windowviewport(rvg_lua_check<window>(L, 1+base),
        rvg_lua_check<viewport>(L, 2+base));
}

static int apply(lua_State *L) {
    xform *xf = rvg_lua_check_pointer<xform>(L, 1);
    auto x = static_cast<rvgf>(luaL_checknumber(L, 2));
    auto y = static_cast<rvgf>(luaL_checknumber(L, 3));
    auto w = static_cast<rvgf>(luaL_optnumber(L, 4, 1.f));
    std::tie(x, y, w) = xf->apply(x, y, w);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, w);
    return 3;
}

static int inverse(lua_State *L) {
    xform *xf = rvg_lua_check_pointer<xform>(L, 1);
    rvg_lua_push<xform>(L, xf->inverse());
    return 1;
}

static int transpose(lua_State *L) {
    xform *xf = rvg_lua_check_pointer<xform>(L, 1);
    rvg_lua_push<xform>(L, xf->transpose());
    return 1;
}

static int adjugate(lua_State *L) {
    xform *xf = rvg_lua_check_pointer<xform>(L, 1);
    rvg_lua_push<xform>(L, xf->adjugate());
    return 1;
}

static int det(lua_State *L) {
    xform *xf = rvg_lua_check_pointer<xform>(L, 1);
    lua_pushnumber(L, xf->det());
    return 1;
}

static luaL_Reg xform__index_table[] = {
    {"apply", apply },
    {"inverse", inverse },
    {"adjugate", adjugate },
    {"transpose", transpose },
    {"det", det },
    { nullptr, nullptr }
};

// xform is special in that it has both methods and
// other data. it receives a table with the methdos as an
// additional upvalue
static int xform__index_func(lua_State *L) {
    if (lua_isnumber(L, 2)) {
        auto i = static_cast<int>(lua_tointeger(L, 2) - 1);
        auto j = i % 3;
        i /= 3;
        if (i >= 0 && i < 3 && j >= 0 && j < 3) {
            lua_pushnumber(L, rvg_lua_check<xform>(L, 1,
                    lua_upvalueindex(2))[i][j]);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_gettable(L, lua_upvalueindex(1));
    }
    return 1;
}

static int xform__len(lua_State *L) {
    lua_pushinteger(L, 6);
    return 1;
}

static int xform__mul(lua_State *L) {
    constexpr int ctxidx = lua_upvalueindex(2);
    rvg_lua_push<xform>(L, rvg_lua_check<xform>(L, 1, ctxidx) *
        rvg_lua_check<xform>(L, 2, ctxidx), ctxidx);
    return 1;
}

static int xform__eq(lua_State *L) {
    constexpr int ctxidx = lua_upvalueindex(2);
    lua_pushboolean(L, rvg_lua_check<xform>(L, 1, ctxidx) ==
        rvg_lua_check<xform>(L, 2, ctxidx));
    return 1;
}

static luaL_Reg xform_meta[] = {
    {"__index", xform__index_func },
    {"__len", xform__len },
    {"__mul", xform__mul },
    {"__eq", xform__eq },
    { nullptr, nullptr }
};

static int create_rotation(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_rotation(L, 1));
}

static int create_scaling(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_scaling(L, 1));
}

static int create_translation(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_translation(L, 1));
}

static int create_windowviewport(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_windowviewport(L, 1));
}

static int create_identity(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_identity(L, 1));
}

static int create_linear(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_linear(L, 1));
}

static int create_affinity(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_affinity(L, 1));
}

static int create_projectivity(lua_State *L) {
    return rvg_lua_push<xform>(L, rvg_lua_xform_projectivity(L, 1));
}

static const luaL_Reg mod_xform[] = {
    {"identity", create_identity},
    {"rotation", create_rotation},
    {"translation", create_translation},
    {"scaling", create_scaling},
    {"linear", create_linear},
    {"affinity", create_affinity},
    {"projectivity", create_projectivity},
    {"windowviewport", create_windowviewport},
    {NULL, NULL}
};

int rvg_lua_xform_init(lua_State *L, int ctxidx) {
	rvg_lua_bbox_init(L, ctxidx);
    if (!rvg_lua_typeexists<xform>(L, ctxidx)) {
		rvg_lua_createtype<xform>(L, "xform", ctxidx);
		rvg_lua_set_xformable<xform>(L, ctxidx);
		rvg_lua_setmethods<xform>(L, xform__index_table, 0, ctxidx);
		// xform is special in that it has both methods and
		// other data. So we take the xform__index_table that
		// setmethods created for us and pass it as an
		// additional upvalue to the xform__index_func that
		// replaces it
		rvg_lua_gettypemetatable<xform>(L, ctxidx);
		lua_getfield(L, -1, "__index");
		rvg_lua_setmetamethods<xform>(L, xform_meta, 1, ctxidx);
		lua_pop(L, 1);
    }
    assert(rvg_lua_typeexists<xform>(L, ctxidx));
    return 0;
}

int rvg_lua_xform_export(lua_State *L, int ctxidx) {
    // mod
    rvg_lua_xform_init(L, ctxidx); // mod
    lua_pushvalue(L, ctxidx); // mod ctxtab
    rvg_lua_setfuncs(L, mod_xform, 1); // mod
    return 0;
}
