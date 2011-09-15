
#include "ORS/OgreDataStream.h"
#include "LuaScriptEngine.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// Компилятор
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"
}

//========================================================================
// Компилятор
//========================================================================

char LuaScriptEngine::Compiler::buf[READ_SIZE] = {0};

const char* LuaScriptEngine::Compiler::reader(lua_State* L, void* data, size_t *size)
{
    (void)L;
    assert( data );
    assert( size );
    DataStream* s = static_cast<DataStream*>(data);
    *size = s->read(buf, READ_SIZE);
    if ( *size )
        return buf;
    return NULL;
}

//------------------------------------------------------------------------

int LuaScriptEngine::Compiler::writer(lua_State* L, const void* p, size_t size, void* data)
{
    (void)L;
    assert( data );
    DataStream* s = static_cast<DataStream*>(data);
    return (s->write(p, size) != size) && (size != 0);
}

//------------------------------------------------------------------------

int LuaScriptEngine::Compiler::dumper(lua_State* L)
{
    Options* opt = static_cast<Options*>(lua_touserdata(L, 1));

    int result = lua_dump(L, Compiler::writer, opt->s, !opt->stripDebug);

    return 0;
}
