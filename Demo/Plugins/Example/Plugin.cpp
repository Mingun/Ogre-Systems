// Plugin.cpp : Определение точки входа DLL приложения.
//

#include "stdafx.h"
#include "Plugin.h"

#include "OLS/OgreLogManager.h"
#include "OPS/PluginManager.h"

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


//Plugin* plugin;
const int PLAGINS_COUNT = 2;
Plugin* plugins[PLAGINS_COUNT];

extern "C" {
PLUGIN_API void dllStartPlugin()
{
    for ( int i = 0; i < PLAGINS_COUNT; ++i )
    {
	    plugins[i] = OGRE_NEW Plugin();
	    Ogre::PluginManager::getSingleton().installPlugin( plugins[i] );
    }
}

PLUGIN_API void dllStopPlugin()
{
    for ( int i = PLAGINS_COUNT-1; i >=0; --i )
    {
	    Ogre::PluginManager::getSingleton().uninstallPlugin( plugins[i] );
	    OGRE_DELETE plugins[i];
    }
}
}

Ogre::String Plugin::mPluginName( "Example plugin" );

Plugin::Plugin()
{
}
Plugin::~Plugin()
{
}
const Ogre::String& Plugin::getName() const
{
	return mPluginName;
}
void Plugin::install()
{
}
void Plugin::initialise()
{
}
void Plugin::shutdown()
{
}
void Plugin::uninstall()
{
}
