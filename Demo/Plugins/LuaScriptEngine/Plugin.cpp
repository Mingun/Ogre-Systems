// Plugin.cpp : Определение точки входа DLL приложения.
//

#include "stdafx.h"
#include "Plugin.h"

#include "OLS/OgreLogManager.h"
#include "OPS/PluginManager.h"
#include "USS/ScriptManager.h"
#include "USS/Lua/LuaScriptEngine.h"

using namespace Ogre;
using namespace Script;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch ( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
    return TRUE;
}


Extension::Plugin* plugin;

extern "C" {
PLUGIN_API void dllStartPlugin()
{
    plugin = OGRE_NEW Extension::Plugin();
	PluginManager::getSingleton().installPlugin( plugin );
}

PLUGIN_API void dllStopPlugin()
{
	PluginManager::getSingleton().uninstallPlugin( plugin );
	OGRE_DELETE plugin;
}
}

namespace Extension {

    String Plugin::mPluginName( "Lua script engine" );
    Plugin::Plugin()
    {
    }
    Plugin::~Plugin()
    {
    }
    const String& Plugin::getName() const
    {
	    return mPluginName;
    }
    void Plugin::install()
    {
    }
    void Plugin::initialise()
    {
        luaEngine.bind(new LuaScriptEngine());

        ScriptManager::getSingleton().registerScriptEngine( luaEngine );
    }
    void Plugin::shutdown()
    {
        ScriptManager::getSingleton().unregisterScriptEngine( luaEngine );

        luaEngine.setNull();
    }
    void Plugin::uninstall()
    {
    }

}// namespace Extension