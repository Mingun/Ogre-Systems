
#include "PluginManager.h"

#include "OES/OgreException.h"
#include "OLS/OgreLogManager.h"
#include "ODLS/OgreDynLibManager.h"
#include "ODLS/OgreDynLib.h"
#include "OPS/OgrePlugin.h"


namespace Ogre {
	//-----------------------------------------------------------------------
	template<> PluginManager* Singleton<PluginManager>::ms_Singleton = 0;
	PluginManager* PluginManager::getSingletonPtr(void)
	{
		return ms_Singleton;
	}
	PluginManager& PluginManager::getSingleton(void)
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	typedef void (*DLL_START_PLUGIN)(void);
	typedef void (*DLL_STOP_PLUGIN)(void);

	//-----------------------------------------------------------------------

	PluginManager::PluginManager(const String& pluginFileName)
        : mLastLoadLib( NULL )
        , mIsInitialised( false )
	{
		// Загружаем плагины
		if (!pluginFileName.empty())
			loadPlugins(pluginFileName);

		LogManager::getSingleton().logMessage("PluginManager Initialising");
	}

	//-----------------------------------------------------------------------
	PluginManager::~PluginManager()
	{
        if (mIsInitialised)
		    shutdownPlugins();
		LogManager::getSingleton().logMessage("PluginManager Shutdown");
		// Практически весь остальной код надо разместить здесь
		unloadPlugins();
	}
	//-----------------------------------------------------------------------
	void PluginManager::installPlugin(Plugin* plugin)
	{
		OGRE_LOCK_AUTO_MUTEX
		LogManager::getSingleton().logMessage("Installing plugin: " + plugin->getName());

        mPlugins.push_back( plugin );
        // Сопостовляем каждый плагин его библиотеке. Здесь мы полагаемся на то,
        // что плагин вызовет installPlugin в своей функции dllStartPlugin.
        mPluginsMap.insert( PluginsMap::value_type(plugin, mLastLoadLib) );
        mPluginLibsMap.insert( PluginLibsMap::value_type(mLastLoadLib, plugin) );
		plugin->install();
        // Если система уже была инициализирована, инициализируем новый плагин.
        if (mIsInitialised)
            plugin->initialise();

		LogManager::getSingleton().logMessage("Plugin successfully installed");
	}
	//-----------------------------------------------------------------------
	void PluginManager::uninstallPlugin(Plugin* plugin)
	{
		OGRE_LOCK_AUTO_MUTEX
		LogManager::getSingleton().logMessage("Uninstalling plugin: " + plugin->getName());

        PluginInstanceList::iterator it = std::find( mPlugins.begin(), mPlugins.end(), plugin );
        if ( it != mPlugins.end() )
		{
            // Если система уже была инициализирована, отключаем плагин.
            if (mIsInitialised)
                plugin->shutdown();
			plugin->uninstall();
            PluginsMap::iterator i = mPluginsMap.find( *it );
            PluginLibsMap::iterator ii = mPluginLibsMap.find( i->second );
            while ( ii->second != i->first )
                ++ii;
            mPluginsMap.erase(i);
            mPluginLibsMap.erase(ii);
			mPlugins.erase(it);
		}
		LogManager::getSingleton().logMessage("Plugin successfully uninstalled");

	}
	//-----------------------------------------------------------------------
	void PluginManager::loadPlugin(const String& pluginName)
	{
		OGRE_LOCK_AUTO_MUTEX
		// Загружаем библиотеку с плагином(ами)
		mLastLoadLib = DynLibManager::getSingleton().load( pluginName );
		// Проверяем, не загружали ли мы ее уже
        if ( std::find(mPluginLibs.begin(), mPluginLibs.end(), mLastLoadLib) == mPluginLibs.end() )
		{
			mPluginLibs.push_back(mLastLoadLib);
			// Вызываем функцию инициализации библиотеки
			DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)mLastLoadLib->getSymbol("dllStartPlugin");

			if (!pFunc)
				OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
                    "Cannot find symbol dllStartPlugin in library " + pluginName,
					"PluginManager::loadPlugin");
			// Она должна вызвать installPlugin
			pFunc();
		}
        mLastLoadLib = NULL;
	}
	//-----------------------------------------------------------------------
	void PluginManager::unloadPlugin(const String& pluginName)
	{
		OGRE_LOCK_AUTO_MUTEX
        PluginLibsList::iterator it = mPluginLibs.begin();
		for ( ; it != mPluginLibs.end(); ++it )
		{
            if ( (*it)->getName() == pluginName )
			{
				// Call plugin shutdown
				DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
				// она должна вызвать uninstallPlugin
				pFunc();
				// Выгружаем библиотеку (уничтожается DynLibManager-ом)
				DynLibManager::getSingleton().unload(*it);
                mPluginLibs.erase(it);
                return;
			}

		}
	}
	//-----------------------------------------------------------------------
	void PluginManager::loadPlugins(const String& pluginsfile)
	{
		/*OGRE_LOCK_AUTO_MUTEX
		StringVector pluginList;
		String pluginDir;
		ConfigFile cfg;

		try {
    		cfg.load( pluginsfile );
		}
		catch (Exception)
		{
			LogManager::getSingleton().logMessage(pluginsfile + " not found, automatic plugin loading disabled.");
			return;
		}

		pluginDir = cfg.getSetting("PluginFolder"); // Ignored on Mac OS X, uses Resources/ directory
		pluginList = cfg.getMultiSetting("Plugin");

	#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
		if (pluginDir.empty())
		{
			// Пользователь не указал папку с плагинами, попробуем в текущей
			pluginDir = ".";
		}
	#endif

		char last_char = pluginDir[pluginDir.length()-1];
		if (last_char != '/' && last_char != '\\')
		{
	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			pluginDir += "\\";
	#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
			pluginDir += "/";
	#endif
		}

		for( StringVector::iterator it = pluginList.begin(); it != pluginList.end(); ++it )
		{
			loadPlugin(pluginDir + (*it));
		}*/

	}
	//-----------------------------------------------------------------------
	void PluginManager::shutdownPlugins(void)
	{
		OGRE_LOCK_AUTO_MUTEX
		// NB Shutdown plugins in reverse order to enforce dependencies
		for ( PluginInstanceList::reverse_iterator it = mPlugins.rbegin(); it != mPlugins.rend(); ++it )
		{
			(*it)->shutdown();
		}
        mIsInitialised = false;
	}
	//-----------------------------------------------------------------------
	void PluginManager::initialisePlugins(void)
	{
		OGRE_LOCK_AUTO_MUTEX
		for ( PluginInstanceList::iterator it = mPlugins.begin(); it != mPlugins.end(); ++it )
		{
			(*it)->initialise();
		}
        mIsInitialised = true;
	}
	//-----------------------------------------------------------------------
	void PluginManager::unloadPlugins(void)
    {
		OGRE_LOCK_AUTO_MUTEX
	    // сначала выгружаем динамические библиотеки
		for ( PluginLibsList::reverse_iterator it = mPluginLibs.rbegin(); it != mPluginLibs.rend(); ++it )
        {
            // Call plugin shutdown
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*it)->getSymbol("dllStopPlugin");
		    // она должна вызвать uninstallPlugin
            pFunc();
            // Выгружаем и уничтожаем библиотеку
            DynLibManager::getSingleton().unload(*it);
        }
        mPluginLibs.clear();

	    // now deal with any remaining plugins that were registered through other means
	    for ( PluginInstanceList::reverse_iterator it = mPlugins.rbegin(); it != mPlugins.rend(); ++it )
	    {
		    // Note this does NOT call uninstallPlugin - this shutdown is for the 
		    // detail objects
		    (*it)->uninstall();
	    }
	    mPlugins.clear();
    }

    DynLib* PluginManager::getLibrary(Plugin* plugin)
    {
		OGRE_LOCK_AUTO_MUTEX

        PluginsMap::iterator it = mPluginsMap.find(plugin);
        if ( it != mPluginsMap.end() )
            return it->second;
        else
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Plugin not found. ", "PluginManager::getLibrary");
    }
}// namespace Ogre