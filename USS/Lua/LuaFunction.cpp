#include "Common/OgrePrerequisites.h"
#include "OES/OgreException.h"

#include "USS/ScriptVar.h"
#include "LuaFunction.h"
#include "LuaBridge.h"
#include "LuaDebug.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

LuaFunction::LuaFunction(lua_State* l, int index)
    : _Base( StringUtil::BLANK ), L(l), mIndex(0)
{
    if ( !lua_isfunction(L, index) )
    {
        StringStream ss;
        ss << "Value at index " << index << " is not function";
        OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS
                   , ss.str()
                   , "LuaFunction::LuaFunction(lua_State* l, int index)" );
        return;
    }
    lua_pushvalue( L, index );              // стек: [func]

    lua_Debug ar;
    if ( lua_getinfo(L, ">Snu", &ar) != 0 )
    {//TODO надо ли проверять на глобальность? На отсутствие upvalues?
        /*if ( ar.nups > 0 )
        {
            OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS
                       , "Function cann't have a upvalues"
                       , "LuaFunction::LuaFunction(lua_State* l, int index)" );
        }*/
        mLine = ar.linedefined;
        if ( ar.source[0] == '@' )
            mFileName = String(ar.source+1);
    }
    mIndex = luaL_ref( L, LUA_REGISTRYINDEX );// стек: []
}

LuaFunction::~LuaFunction()
{
    luaL_unref( L, LUA_REGISTRYINDEX, mIndex );
}

void LuaFunction::eval(const ScriptVarList& inArgs, ScriptVarList& outArgs) const 
{
    // стек: []
    lua_rawgeti( L, LUA_REGISTRYINDEX, mIndex ); // стек: [func]
    assert( lua_isfunction(L, -1) );

    inArgs.pack( LuaBridge(L) );         // стек: [func ...]
    lua_call( L, inArgs.size(), outArgs.size() );// стек: [func ...]
    outArgs.unpack( LuaBridge(L, 1) );      // стек: [func ...]
    lua_pop( L, static_cast<int>(outArgs.size())+1 ); // стек: []
}
