
#ifndef table2_h
#define table2_h

#include "assert.h"

#define copy_args(L, n) \
  {int i;for (i = n; i <= nargs; ++i) lua_pushvalue(L, i);}

#define find_boundaries(L) \
  if (lua_type(L, 2) != LUA_TNUMBER) {\
    first = 1;\
    arg1  = 2;\
  } else\
  if (lua_type(L, 3) != LUA_TNUMBER) {\
    first = 1;\
    last  = lua_tointeger(L, 2);\
    arg1  = 3;\
  } else {\
    first = lua_tointeger(L, 2);\
    last  = lua_tointeger(L, 3);\
    arg1  = 4;\
  }

#define find_last(L)\
  if (last == 0) {\
    for (i = arg1; i <= nargs; ++i) {\
      luaL_checktype(L, i, LUA_TTABLE);\
      n = aux_getn(L, i);\
      if (n > last) last = n;\
    }\
  }

#define geti(L, t, i) (lua_pushinteger(L, i), lua_gettable(L, t))
/** ���������, �������� �� ������ �������� �������� ��� ����� �� �� ��������� __call. */
static void check_1st(lua_State *L) {
  if (lua_isfunction(L, 1)) {
    return;
  } else if (luaL_getmetafield(L, 1, "__call")) {
    lua_pop(L, 1);
  } else {
    luaL_typerror(L, 1, lua_typename(L, LUA_TFUNCTION));
  }
}

/** [-(n+1) +1|0] */
static int call(lua_State *L, int n) {
  /* ����: [... f ...]*/
  /* ����� ������� � `n' ����������� � 1 ����������� */
  if (lua_isfunction(L, 1)) {
    lua_call(L, n, 1);      /* ����: [... result|nil] */
  } else {
    luaL_getmetafield(L, 1, "__call");/* ����: [... o ... __call] */
    lua_insert(L, -(n+2));  /* ����: [... __call o ...] */
    lua_call(L, n+1, 1);    /* ����: [... result|nil] */
  }
  if (!lua_isnil(L, -1))/* ���� ������� ������� ���-��, �������� �� nil, ����� */
    return 1;           /* ����: [... result] */
  lua_pop(L, 1);        /* ����: [...] ������� nil �� ����� */
  return 0;
}

/*----------------------------------------------------------*/

static int table_transpose(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_newtable(L);      /* ����: [t tt] */
  lua_pushnil(L);       /* ����: [t tt nil] */
  while (lua_next(L, 1)) {/* ����: [t tt key value] */
    lua_pushvalue(L, -2); /* ����: [t tt key value key] */
    lua_rawset(L, 2);   /* ����: [t tt key] tt[value]=key */
  }   
  return 1;             /* ����: [t tt] */
}

/**
iforeach(f[[, first], last], t1[, t2[,...]]) 
[-0 +1]
*/
static int table_iforeach(lua_State *L) {
  int nargs = lua_gettop(L);/* ����: [...] */
  ptrdiff_t first = 0;
  ptrdiff_t last  = 0;
  int arg1;/* ������ ������ �������. */
  int n;
  int i;

  check_1st(L);
  /* 1 - ���� �������, ���� ��� ����. */
  find_boundaries(L);
  lua_assert(nargs > arg1);
  /* 2 - ���������� ������� �������, ���� ��� �� ������. */
  find_last(L);

  /* 3 - ��������� ��������. */
  n = nargs - arg1 + 1;
  for (; first <= last; ++first) {/* ����: [...] */
    lua_pushvalue(L, 1);    /* ����: [... f] */

    /* ��������� ������ ���������� ������� */
    for (i = arg1; i <= nargs; ++i) {
      /* �������� �������� �� ������� (��������, ����� ����������) */
      geti(L, i, first);    /* ����: [... f ti[first]] */
    }                       /* ����: [... f ...] */

    if (call(L, n))         /* ����: [... result] */
      return 1;
    /* ����: [...] */
  }
  assert(nargs == lua_gettop(L));
  return 0;
}
/** [-0 +0] */
static int imap_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int t = lua_upvalueindex(2);
  int i = lua_upvalueindex(3);
  int nargs = lua_gettop(L);/* ����: [...] */

  lua_pushvalue(L, f);      /* ����: [... f] */
  copy_args(L, 1);          /* ����: [... f ...] */
  lua_call(L, nargs, 1);    /* ����: [... x] x = f(...) */

  lua_pushvalue(L, i);      /* ����: [... x idx] */

  lua_insert(L, -2);        /* ����: [... idx x] */
  lua_rawset(L, t);         /* ����: [...] t[idx] = x */

  lua_pushinteger(L, lua_tointeger(L, i)+1);/* ����: [... idx+1] */
  lua_replace(L, i);        /* ����: [...] ++idx */

  assert(nargs == lua_gettop(L));
  return 0;
}
/** [-0 +1] */
static int table_imap(lua_State *L) {
  int nargs = lua_gettop(L);/* ����: [f ...] */

  lua_newtable(L);      /* ����: [f ... result] */
  lua_pushcfunction(L, table_iforeach);/* ����: [f ... result iforeach] */

  lua_pushvalue(L, 1);  /* ����: [f ... result iforeach f] */
  lua_pushvalue(L, -3); /* ����: [f ... result iforeach f result] */
  lua_pushinteger(L, 1);/* ����: [f ... result iforeach f result index] */
  lua_pushcclosure(L, imap_helper, 3);/* ����: [f ... result iforeach g] */

  copy_args(L, 2);      /* ����: [f ... result iforeach g ...] */
  lua_call(L, nargs, 0);/* ����: [f ... result] */

  assert(nargs == lua_gettop(L)-1);
  return 1;
}

/** [-0 +0] */
static int ifold_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int acc = lua_upvalueindex(2);
  int nargs = lua_gettop(L);/* ����: [...] */

  lua_pushvalue(L, f);      /* ����: [... f] */
  lua_pushvalue(L, acc);    /* ����: [... f acc] */
  copy_args(L, 1);          /* ����: [... f acc ...] */

  lua_call(L, nargs+1, 1);  /* ����: [... x] x = f(acc, ...) */

  lua_replace(L, acc);      /* ����: [...] acc = x */

  assert(nargs == lua_gettop(L));
  return 0;
}

static int table_ifold(lua_State *L) {
  int nargs = lua_gettop(L);/* ����: [f acc ...] */

  lua_pushcfunction(L, table_iforeach);/* ����: [f acc ... iforeach] */

  lua_pushvalue(L, 1);      /* ����: [f acc ... iforeach f] */
  lua_pushvalue(L, 2);      /* ����: [f acc ... iforeach f acc] */
  lua_pushcclosure(L, ifold_helper, 2);/* ����: [f acc ... iforeach g] */

  /* ��������� ��������� ���������, ����� ����� �������� ���������*/
  lua_pushvalue(L, -1);     /* ����: [f acc ... iforeach g g] */
  lua_insert(L, -3);        /* ����: [f acc ... g iforeach g] */

  copy_args(L, 3);          /* ����: [f acc ... g iforeach g ...] */
  lua_call(L, nargs-1, 0);  /* ����: [f acc ... g] */
  lua_getupvalue(L, -1, 2); /* ����: [f acc ... g result] */
  lua_remove(L, -2);        /* ����: [f acc ... result] */
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}
/** [-0 +1] */
static int izip_helper(lua_State *L) {
  int nargs = lua_gettop(L);    /* ����: [...] */
  int i;

  lua_createtable(L, nargs, 0); /* ����: [... t] */

  for (i = 1; i <= nargs; ++i) {
    lua_pushvalue(L, i);        /* ����: [... t x] */
    lua_rawseti(L, -2, i);      /* ����: [... t] t[i] = x */
  }
  assert(nargs == lua_gettop(L)-1);
  return 1;                     /* ����: [... t] */
}

static int table_izip(lua_State *L) {
  int nargs = lua_gettop(L);/* ����: [...] */

  lua_pushcfunction(L, table_imap); /* ����: [... imap] */

  lua_pushcfunction(L, izip_helper);/* ����: [... imap g] */
  copy_args(L, 1);          /* ����: [... imap g ...] */
  lua_call(L, nargs+1, 1);  /* ����: [... result] */
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}

/** 
[-0 +1] 
local function g(...) return not f(...) end
*/
static int iall_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int nargs = lua_gettop(L);/* ����: [...] */
  
  lua_pushvalue(L, f);      /* ����: [... f] */
  copy_args(L, 1);          /* ����: [... f ...] */

  lua_call(L, nargs, 1);    /* ����: [... x] x = f(...) */
  lua_pushboolean(L, !lua_toboolean(L, -1));/* ����: [... x (not x)] return not f(...) */
  lua_remove(L, -2);
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}

static int table_iall(lua_State *L) {
  int nargs = lua_gettop(L);/* ����: [f ...] */

  lua_pushcfunction(L, table_iforeach); /* ����: [f ... iforeach] */

  lua_pushvalue(L, 1);      /* ����: [f ... iforeach f] */
  lua_pushcclosure(L, iall_helper, 1);  /* ����: [f ... iforeach g] */
  copy_args(L, 2);          /* ����: [f ... iforeach g ...] */

  lua_call(L, nargs, 1);    /* ����: [f ... result] */
  lua_pushboolean(L, !lua_toboolean(L, -1));/* ����: [... x (not x)] return not iforeach(g,...) */
  lua_remove(L, -2);

  assert(nargs == lua_gettop(L)-1);
  return 1;
}

/*#define table_ifilter table_iforeach
#define table_icat table_iforeach
#define table_iflatten table_iforeach
#define table_irev table_iforeach
#define table_isub table_iforeach*/
#define table_iany table_iforeach
/*
#define table_cat table_icat
#define table_range table_iforeach*/

#endif
