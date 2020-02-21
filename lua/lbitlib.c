/* Bitwise operations library */
/* Reuben Thomas   nov00-09jan04 */

#include "lauxlib.h"
#include "lua.h"

typedef long long Integer;
typedef unsigned long long UInteger;

#define luaL_checkbit(L, n)  ((Integer)luaL_checknumber(L, n))
#define luaL_checkubit(L, n) ((UInteger)luaL_checkbit(L, n))

#define TDYADIC(name, op, checkbit1, checkbit2) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, \
      checkbit1(L, 1) op checkbit2(L, 2)); \
    return 1; \
  }

#define DYADIC(name, op) \
  TDYADIC(name, op, luaL_checkbit, luaL_checkbit)

#define MONADIC(name, op) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, op luaL_checkbit(L, 1)); \
    return 1; \
  }

#define VARIADIC(name, op) \
  static int bit_ ## name(lua_State *L) { \
    int n = lua_gettop(L), i; \
    Integer w = luaL_checkbit(L, 1); \
    for (i = 2; i <= n; i++) \
      w op luaL_checkbit(L, i); \
    lua_pushnumber(L, w); \
    return 1; \
  }

MONADIC(bnot,     ~)
VARIADIC(band,    &=)
VARIADIC(bor,     |=)
VARIADIC(bxor,    ^=)
TDYADIC(lshift,  <<, luaL_checkbit, luaL_checkubit)
TDYADIC(rshift,  >>, luaL_checkubit, luaL_checkubit)
TDYADIC(arshift, >>, luaL_checkbit, luaL_checkubit)
DYADIC(mod,      %)

static const struct luaL_reg bitlib[] = {
  {"bnot",    bit_bnot},
  {"band",    bit_band},
  {"bor",     bit_bor},
  {"bxor",    bit_bxor},
  {"lshift",  bit_lshift},
  {"rshift",  bit_rshift},
  {"arshift", bit_arshift},
  {"mod",     bit_mod},
  {NULL, NULL}
};

LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_openlib(L, "bit", bitlib, 0);
  return 1;
}
