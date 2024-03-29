#ifndef __USSLuaBridge_H__
#define __USSLuaBridge_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreStringConverter.h"
#include "USS/ScriptVar.h"
#include "USS/ScriptVarList.h"
#include "LuaFunction.h"
#include "LuaScriptEngine.h"

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
    // Группа методов для передачи абстрактных аргументов
    // в структуры скриптового движка.
    virtual void pack(bool value)   { lua_pushboolean(L, value); }
    virtual void pack(int value)    { lua_pushnumber (L, value); }
    virtual void pack(double value) { lua_pushnumber (L, value); }
    virtual void pack(char value)   { char ch[] = {value, 0}; lua_pushstring( L, ch ); }
    virtual void pack(const String& value) { lua_pushlstring( L, value.c_str(), value.size() ); }
    virtual void pack(const Iterator* value) {
        lua_pushnil(L);
        //LuaScriptEngine::Iterator::push(L, value);
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
    virtual void pack(const IScriptablePtr& value) {
        engine(L)->pushObject(value);
    }
    //FIXME Реализовать pack(const Function* value)!
    virtual void pack(const Function* value) {
        lua_pushnil(L);
    }
    virtual void pack(const void* value) { lua_pushlightuserdata(L, const_cast<void*>(value)); }

    // Группа методов для передачи агрументов из внутренних структур
    // скриптового движка в абстрактные аргументы.
    virtual void unpack(bool& value)   { value = static_cast<bool>(lua_toboolean(L, mIndex)); }
    virtual void unpack(int& value)    { value = static_cast<int >(lua_tointeger(L, mIndex)); }
    virtual void unpack(double& value) { value = lua_tonumber (L, mIndex); }
    virtual void unpack(char& value)   { 
        const char* val = lua_tostring(L, mIndex);
        if (val) value = val[0]; 
        else value = 0;
        //value = lua_tostring (L, mIndex)[0]; 
    }
    virtual void unpack(String& value) { 
        // Если вдруг пришла не строка, lua_tolstring вернет NULL
        size_t len = 0;
        const char* val = lua_tolstring (L, mIndex, &len);
        if (val) value = String(val, val+len); 
        else value = StringUtil::BLANK;
    }
    virtual void unpack(Iterator*& value) {
        //value = LuaScriptEngine::Iterator::check(L, mIndex);
    }
    virtual void unpack(IScriptablePtr& value) {
        value = LuaScriptEngine::getScriptable(L, mIndex);
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
        //else if ( lua_isstring(L, mIndex) )
        //    value = new FileFunction(lua_tostring(L, mIndex));
        else if ( lua_isnil(L, mIndex) )
            value = NULL;
    }
    virtual void unpack(void*& value)  { value = lua_touserdata(L, mIndex); }

    // Группа методов для проверки соответствия ожидаемых аргументов
    // фактическим аргументам, содержащихся во внутренних структурах
    // скриптового движка.

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
    /// @c String эквивалентно Lua типу @c string и частично Lua типу @c number.
    virtual int check(const String& value) {
        if ( lua_type(L, mIndex) == LUA_TSTRING )
            return 2;
        if ( lua_type(L, mIndex) == LUA_TNUMBER )
            return 1;
        return 0;
    }
    virtual int check(const Iterator* value) {
        return 0;//LuaScriptEngine::Iterator::check(L, ++mIndex);
    }
    virtual int check(const IScriptablePtr& value) {
        return static_cast<int>(LuaScriptEngine::getScriptable(L, mIndex).isNull()) << 1;
    }
    virtual int check(const Hash& value) {
        return lua_istable(L, mIndex) << 1;
    }
    virtual int check(const Function* value) {
        return lua_isfunction(L, mIndex) << 1
            //|| lua_isstring(L, mIndex) 
            || lua_isnil(L, mIndex) 
            //|| lua_islightuserdata(L, mIndex)
            ;
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

#endif // __USSLuaBridge_H__
