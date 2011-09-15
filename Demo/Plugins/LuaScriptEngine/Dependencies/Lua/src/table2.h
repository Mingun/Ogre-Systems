
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
/** Проверяет, является ли первый агрумент функцией или имеет ли он метаметод __call. */
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
  /* стек: [... f ...]*/
  /* вызов функции с `n' аргументами и 1 результатом */
  if (lua_isfunction(L, 1)) {
    lua_call(L, n, 1);      /* стек: [... result|nil] */
  } else {
    luaL_getmetafield(L, 1, "__call");/* стек: [... o ... __call] */
    lua_insert(L, -(n+2));  /* стек: [... __call o ...] */
    lua_call(L, n+1, 1);    /* стек: [... result|nil] */
  }
  if (!lua_isnil(L, -1))/* если функция вернула что-то, отличное от nil, выйти */
    return 1;           /* стек: [... result] */
  lua_pop(L, 1);        /* стек: [...] удалить nil из стека */
  return 0;
}

/*----------------------------------------------------------*/

static int table_transpose(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_newtable(L);      /* стек: [t tt] */
  lua_pushnil(L);       /* стек: [t tt nil] */
  while (lua_next(L, 1)) {/* стек: [t tt key value] */
    lua_pushvalue(L, -2); /* стек: [t tt key value key] */
    lua_rawset(L, 2);   /* стек: [t tt key] tt[value]=key */
  }   
  return 1;             /* стек: [t tt] */
}

/**
iforeach(f[[, first], last], t1[, t2[,...]]) 
[-0 +1]
*/
static int table_iforeach(lua_State *L) {
  int nargs = lua_gettop(L);/* стек: [...] */
  ptrdiff_t first = 0;
  ptrdiff_t last  = 0;
  int arg1;/* индекс первой таблицы. */
  int n;
  int i;

  check_1st(L);
  /* 1 - ищем границы, если они есть. */
  find_boundaries(L);
  lua_assert(nargs > arg1);
  /* 2 - определяем верхнюю границу, если она не задана. */
  find_last(L);

  /* 3 - выполняем итерации. */
  n = nargs - arg1 + 1;
  for (; first <= last; ++first) {/* стек: [...] */
    lua_pushvalue(L, 1);    /* стек: [... f] */

    /* формируем список аргументов функции */
    for (i = arg1; i <= nargs; ++i) {
      /* получаем значение из таблицы (возможно, через метаметоды) */
      geti(L, i, first);    /* стек: [... f ti[first]] */
    }                       /* стек: [... f ...] */

    if (call(L, n))         /* стек: [... result] */
      return 1;
    /* стек: [...] */
  }
  assert(nargs == lua_gettop(L));
  return 0;
}
/** [-0 +0] */
static int imap_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int t = lua_upvalueindex(2);
  int i = lua_upvalueindex(3);
  int nargs = lua_gettop(L);/* стек: [...] */

  lua_pushvalue(L, f);      /* стек: [... f] */
  copy_args(L, 1);          /* стек: [... f ...] */
  lua_call(L, nargs, 1);    /* стек: [... x] x = f(...) */

  lua_pushvalue(L, i);      /* стек: [... x idx] */

  lua_insert(L, -2);        /* стек: [... idx x] */
  lua_rawset(L, t);         /* стек: [...] t[idx] = x */

  lua_pushinteger(L, lua_tointeger(L, i)+1);/* стек: [... idx+1] */
  lua_replace(L, i);        /* стек: [...] ++idx */

  assert(nargs == lua_gettop(L));
  return 0;
}
/** [-0 +1] */
static int table_imap(lua_State *L) {
  int nargs = lua_gettop(L);/* стек: [f ...] */

  lua_newtable(L);      /* стек: [f ... result] */
  lua_pushcfunction(L, table_iforeach);/* стек: [f ... result iforeach] */

  lua_pushvalue(L, 1);  /* стек: [f ... result iforeach f] */
  lua_pushvalue(L, -3); /* стек: [f ... result iforeach f result] */
  lua_pushinteger(L, 1);/* стек: [f ... result iforeach f result index] */
  lua_pushcclosure(L, imap_helper, 3);/* стек: [f ... result iforeach g] */

  copy_args(L, 2);      /* стек: [f ... result iforeach g ...] */
  lua_call(L, nargs, 0);/* стек: [f ... result] */

  assert(nargs == lua_gettop(L)-1);
  return 1;
}

/** [-0 +0] */
static int ifold_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int acc = lua_upvalueindex(2);
  int nargs = lua_gettop(L);/* стек: [...] */

  lua_pushvalue(L, f);      /* стек: [... f] */
  lua_pushvalue(L, acc);    /* стек: [... f acc] */
  copy_args(L, 1);          /* стек: [... f acc ...] */

  lua_call(L, nargs+1, 1);  /* стек: [... x] x = f(acc, ...) */

  lua_replace(L, acc);      /* стек: [...] acc = x */

  assert(nargs == lua_gettop(L));
  return 0;
}

static int table_ifold(lua_State *L) {
  int nargs = lua_gettop(L);/* стек: [f acc ...] */

  lua_pushcfunction(L, table_iforeach);/* стек: [f acc ... iforeach] */

  lua_pushvalue(L, 1);      /* стек: [f acc ... iforeach f] */
  lua_pushvalue(L, 2);      /* стек: [f acc ... iforeach f acc] */
  lua_pushcclosure(L, ifold_helper, 2);/* стек: [f acc ... iforeach g] */

  /* сохраняем экземпляр замыкания, чтобы потом получить результат*/
  lua_pushvalue(L, -1);     /* стек: [f acc ... iforeach g g] */
  lua_insert(L, -3);        /* стек: [f acc ... g iforeach g] */

  copy_args(L, 3);          /* стек: [f acc ... g iforeach g ...] */
  lua_call(L, nargs-1, 0);  /* стек: [f acc ... g] */
  lua_getupvalue(L, -1, 2); /* стек: [f acc ... g result] */
  lua_remove(L, -2);        /* стек: [f acc ... result] */
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}
/** [-0 +1] */
static int izip_helper(lua_State *L) {
  int nargs = lua_gettop(L);    /* стек: [...] */
  int i;

  lua_createtable(L, nargs, 0); /* стек: [... t] */

  for (i = 1; i <= nargs; ++i) {
    lua_pushvalue(L, i);        /* стек: [... t x] */
    lua_rawseti(L, -2, i);      /* стек: [... t] t[i] = x */
  }
  assert(nargs == lua_gettop(L)-1);
  return 1;                     /* стек: [... t] */
}

static int table_izip(lua_State *L) {
  int nargs = lua_gettop(L);/* стек: [...] */

  lua_pushcfunction(L, table_imap); /* стек: [... imap] */

  lua_pushcfunction(L, izip_helper);/* стек: [... imap g] */
  copy_args(L, 1);          /* стек: [... imap g ...] */
  lua_call(L, nargs+1, 1);  /* стек: [... result] */
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}

/** 
[-0 +1] 
local function g(...) return not f(...) end
*/
static int iall_helper(lua_State *L) {
  int f = lua_upvalueindex(1);
  int nargs = lua_gettop(L);/* стек: [...] */
  
  lua_pushvalue(L, f);      /* стек: [... f] */
  copy_args(L, 1);          /* стек: [... f ...] */

  lua_call(L, nargs, 1);    /* стек: [... x] x = f(...) */
  lua_pushboolean(L, !lua_toboolean(L, -1));/* стек: [... x (not x)] return not f(...) */
  lua_remove(L, -2);
  
  assert(nargs == lua_gettop(L)-1);
  return 1;
}

static int table_iall(lua_State *L) {
  int nargs = lua_gettop(L);/* стек: [f ...] */

  lua_pushcfunction(L, table_iforeach); /* стек: [f ... iforeach] */

  lua_pushvalue(L, 1);      /* стек: [f ... iforeach f] */
  lua_pushcclosure(L, iall_helper, 1);  /* стек: [f ... iforeach g] */
  copy_args(L, 2);          /* стек: [f ... iforeach g ...] */

  lua_call(L, nargs, 1);    /* стек: [f ... result] */
  lua_pushboolean(L, !lua_toboolean(L, -1));/* стек: [... x (not x)] return not iforeach(g,...) */
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
