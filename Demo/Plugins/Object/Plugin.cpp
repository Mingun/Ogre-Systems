// Plugin.cpp : Определение точки входа DLL приложения.
//

#include "stdafx.h"
#include "Plugin.h"

#include "OLS/OgreLogManager.h"
#include "OPS/PluginManager.h"
#include "USS/ScriptManager.h"
#include "USS/demo/Pair/Pair.h"
#include "USS/demo/Pair/PairClass.h"
#include "USS/demo/Pair/PairObject.h"

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
	Ogre::PluginManager::getSingleton().installPlugin( plugin );
}

PLUGIN_API void dllStopPlugin()
{
	Ogre::PluginManager::getSingleton().uninstallPlugin( plugin );
	OGRE_DELETE plugin;
}
}

namespace Extension {

    Ogre::String Plugin::mPluginName( "Scriptable Pair" );

    Plugin::Plugin()
        //: mClass(new test::PairClass())
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
        mClass.bind(new test::PairClass());

        mObj.bind(new test::PairObject(mClass.get()));

        Script::ScriptManager::getSingleton().registerClass(mClass);
        Script::ScriptManager::getSingleton().registerObject("cppobject", mObj);
    }
    void Plugin::shutdown()
    {
        Script::ScriptManager::getSingleton().unregisterObject("cppobject");
        Script::ScriptManager::getSingleton().unregisterClass("Pair");

        mObj.setNull();
        mClass.setNull();
    }
    void Plugin::uninstall()
    {
    }

}// namespace Extension