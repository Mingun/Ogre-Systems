#ifndef __LuaBridge_H__
#define __LuaBridge_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreStringConverter.h"
#include "USS/ScriptVar.h"
#include "USS/ScriptVarList.h"
#include "LuaFunction.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#   pragma warning(push)
    //forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning(disable : 4800)
#endif

extern "C" {
#include "lua.h"
}
using namespace Script;

class LuaBridge : public Bridge
{
protected:
    lua_State* L;
    /// Индекс в стеке для проверки или извлечения.
    mutable int mIndex;
    /// Размер стека на момент создания моста.
    //int mStackSize;
public:
    LuaBridge(lua_State* _L, int idx = 0) : L( _L ), mIndex( idx ) {
        //mStackSize = idx > 0 ? lua_gettop(_L) : -lua_gettop(_L);
    }
    virtual void pack(bool value)   { lua_pushboolean(L, value); }
    virtual void pack(int value)    { lua_pushnumber (L, value); }
    virtual void pack(double value) { lua_pushnumber (L, value); }
    virtual void pack(char value)   { char ch[] = {value, 0}; lua_pushstring( L, ch ); }
    virtual void pack(const String& value) { lua_pushlstring( L, value.c_str(), value.size() ); }
    virtual void pack(const Iterator* value) {
        //LuaScriptEnginePrivate::Iterator::push(L, value);
    }
    virtual void pack(const Hash& value) {
        lua_createtable( L, 0, static_cast<int>(value.size()) );
        for ( Hash::const_iterator it = value.begin(); it != value.end(); ++it )
        {
            lua_pushlstring( L, it->first.c_str(), it->first.size() );
            it->second->pack( *this );
            lua_rawset( L, -2 );
        }
    }
    virtual void pack(const IScriptable* value) {}// LuaScriptEnginePrivate::pushScriptable(L, value); }
    //FIXME Реализовать pack(const Function* value)!
    virtual void pack(const Function* value) {  }
    virtual void pack(const void* value) { lua_pushlightuserdata(L, const_cast<void*>(value)); }

    virtual void unpack(bool& value)   { value = static_cast<bool>(lua_toboolean(L, mIndex)); }
    virtual void unpack(int& value)    { value = static_cast<int >(lua_tointeger(L, mIndex)); }
    virtual void unpack(double& value) { value = lua_tonumber (L, mIndex); }
    virtual void unpack(char& value)   { value = lua_tostring (L, mIndex)[0]; }
    virtual void unpack(String& value) { value = lua_tostring (L, mIndex); }
    virtual void unpack(Iterator*& value) {
        //value = LuaScriptEnginePrivate::Iterator::check(L, mIndex);
    }
    virtual void unpack(IScriptable*& value) {
        //value = LuaScriptEnginePrivate::checkScriptable(L, --mIndex);
    }
    virtual void unpack(Hash& value) {
        LuaBridge bridge(L, -1);
        for ( Hash::iterator it = value.begin(); it != value.end(); ++it )
        {
            lua_getfield( L, mIndex, it->first.c_str() );
            it->second->unpack( bridge );
            lua_pop(L, 1);
        }
        /*
        lua_pushnil(L);  // первый ключ
        while ( lua_next(L, mIndex) != 0 )
        {
            if ( lua_isstring(L, -2) )
            {
                ScriptVar* var = unpack();
                value.insert( Hash::value_type( lua_tostring(L, 2), var ) );
            }
            lua_pop(L, 1);
        }*/
    }
    virtual void unpack(Function*& value) {
        if ( lua_isfunction(L, mIndex) )
            value = new LuaFunction(L, mIndex);
        else if ( lua_isstring(L, mIndex) )
            value = new FileFunction(lua_tostring(L, mIndex));
        else if ( lua_isstring(L, mIndex) )
            value = new FileFunction(lua_tostring(L, mIndex), "main");
    }
    virtual void unpack(void*& value)  { value = lua_touserdata(L, mIndex); }

    /// @c bool эквивалентно только Lua типу @c boolean.
    virtual int check(bool value)  { return lua_isboolean(L, mIndex) << 1; }
    /// @c int эквивалентно Lua типу @c number, если в нем записано целое число
    /// и частично Lua типу @c string, если он представляет строку или Lua типу 
    /// @c number, если число дробное.
    virtual int check(int value)   { 
        if (!lua_isnumber (L, mIndex))
            return 0;
        if (lua_type(L, mIndex) == LUA_TSTRING)
            return 1;
        return 1 + (lua_tonumber(L, mIndex) == lua_tointeger(L, mIndex));
    }
    /// @c double эквивалентно Lua типу @c number и частично Lua типу @c string, если он представляет строку.
    virtual int check(double value){ return lua_isnumber(L, mIndex) + (lua_type(L, mIndex) == LUA_TNUMBER); }
    /// @c char эквивалентно только Lua типу @c string, объект которого имеет только один символ.
    virtual int check(char value)  { 
        if ( lua_type(L, mIndex) != LUA_TSTRING )
            return 0;
        size_t len = 0;
        lua_tolstring(L, mIndex, &len);
        return len == 1;
    }
    /// String эквивалентно Lua типу @c string и частично Lua типу @c number.
    virtual int check(const String& value) {
        if ( lua_type(L, mIndex) == LUA_TSTRING )
            return 2;
        if ( lua_type(L, mIndex) == LUA_TNUMBER )
            return 1;
        return 0;
    }
    virtual int check(const Iterator* value) {
        return 0;//LuaScriptEnginePrivate::Iterator::check(L, ++mIndex);
    }
    virtual int check(const IScriptable* value) {
        return 0;//LuaScriptEnginePrivate::checkScriptable(L, ++mIndex);
    }
    virtual int check(const Hash& value) {
        return lua_istable(L, mIndex) << 1;
    }
    virtual int check(const Function* value) {
        return lua_isfunction(L, mIndex)
            || lua_isstring(L, mIndex) 
            /*|| lua_isuserdata(L, mIndex) 
            || lua_islightuserdata(L, mIndex)*/;
    }
    virtual int check(const void* value) {
        return (lua_isuserdata(L, mIndex) || lua_islightuserdata(L, mIndex)) << 1;
    }
    virtual int check(const ScriptVarList& vars) { return vars.size() == lua_gettop(L)-mIndex; }

    virtual void next() { ++mIndex; }

    //virtual bool isEmpty() const { return mIndex > mStackSize; }
    virtual bool isEmpty() const { return lua_isnone(L, mIndex); }

    virtual Bridge* clone() const
    {
        return new LuaBridge(L, mIndex);
    }
};

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    //forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning(pop)
#endif

#endif // __LuaBridge_H__
