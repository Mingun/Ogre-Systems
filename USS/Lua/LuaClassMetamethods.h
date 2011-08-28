#include "USS/IConstructor.h"

#ifdef RegisterFunctionForLua
#undef RegisterFunctionForLua
#endif
#define RegisterFunctionForLua(name) \
    {#name, LuaScriptEngine::Class::name}

luaL_Reg LuaScriptEngine::Class::mt[] = {
    RegisterFunctionForLua(__call)
  //, {"__gc", LuaScriptEngine::Class::__GC}
  //, RegisterFunctionForLua(__index)
  //, RegisterFunctionForLua(__newindex)
  , {"__tostring", LuaScriptEngine::__tostring}

  , {NULL, NULL}
};

int LuaScriptEngine::Class::__index(lua_State *L)
{
    return 0;
}

//------------------------------------------------------------------------
int LuaScriptEngine::Class::__newindex(lua_State *L)
{
    return 0;
}
//------------------------------------------------------------------------
int LuaScriptEngine::Class::__call(lua_State *L)
{
    IClass* c = getClass(L, 1);
    ConstructorList constructors = c->getConstructors();
    IConstructor* cons = findConstructor( constructors, LuaBridge(L, 2) );
    ScriptVarListPtr inArgs;
    if ( !cons )
    {
        cons = findConstructorTable( L, constructors );
        if ( !cons )
        {
            luaL_error(L, "no constructor avalible");
            return 0;
        }
        inArgs = cons->inArgs();
        std::for_each( inArgs->begin(), inArgs->end(), TableUnpacker(L, -1) );
    } else {
        inArgs = cons->inArgs();
        inArgs->unpack( LuaBridge(L, 2) );
    }
    pushObject( L, cons->create(*inArgs) );
    return 1;
}
//------------------------------------------------------------------------
int LuaScriptEngine::Class::__GC(lua_State *L)
{
    return 0;
}