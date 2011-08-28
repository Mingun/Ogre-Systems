#ifndef __LuaDebug_H__
#define __LuaDebug_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "Common/OgreStringConverter.h"
#include "OLS/OgreLogManager.h"

#define _s(x) #x
#define fn(x) __FUNCTION__ ## ":" ## _s(x)

#define TRACE(x)    LOG(fn(__LINE__)),LOG(x)
#define TRACE_IN    Script::Tracer __tracer__(__FUNCTION__)
#define TRACE_STACK TRACE( getStackTypes(L) )

extern "C" {
#include "lua.h"
}
namespace Script {
    /// Трассировщик входов / выходов в функции
    class Tracer
    {
        Ogre::String func;
        static char indent[20];
        static int  last;
        static char filler;
    public:
        Tracer(const Ogre::String& fun) 
            : func( fun )
        {
            TRACE( indent + func );
            indent[++last] = filler;
        }
        virtual ~Tracer() 
        {
            indent[last--] = 0;
            TRACE( indent + ("~" + func) );
        }
    };

class Printer
{
    Ogre::Log::Stream stream;
public:
    Printer() : stream( Ogre::LogManager::getSingleton().stream() ) {}
    template<typename T>
    void operator()(const T* arg)
    {
        stream << arg->toString();
    }
};

Ogre::String getStackTypes(lua_State* L);

} // namespace Script

#endif // __LuaDebug_H__
