#include "Common/OgrePrerequisites.h"
#include "Common/OgreStringConverter.h"

#include "LuaDebug.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
namespace Script {

    using namespace Ogre;

    char Tracer::indent[] = {0};
    int  Tracer::last = -1;
    char Tracer::filler = ' ';

    void tostring(StringStream& ss, lua_State* L, int i)
    {
        if (luaL_callmeta(L, i, "__tostring"))
        {
            ss << lua_tostring(L, -1);
            lua_pop(L, 1);
            return;
        }
        switch ( lua_type(L, i) ) 
        {
        case LUA_TNUMBER:
            lua_pushvalue(L, i);
            ss << lua_tostring(L, -1);
            lua_pop(L, 1);
            break;
        case LUA_TSTRING:
            ss << lua_tostring(L, i);
            break;
        case LUA_TBOOLEAN:
            ss << StringStream::boolalpha << lua_toboolean(L, i);
            break;
        case LUA_TNIL:
            ss << "nil";
            break;
        default:
            ss << "0x" << lua_topointer(L, i);
            break;
        }
    }
    String getStackTypes(lua_State* L)
    {
        StringStream ss;
        ss << "Lua stack:\n";
        int i = lua_gettop(L);
        while ( i )
        {
            ss << StringConverter::toString(i, 2) << ". [" << lua_typename(L, lua_type(L, i));
            ss << "]\t";
            tostring(ss, L, i);
            ss << "\n";
            --i;
        }
        return ss.str();
    }

} // namespace Script
