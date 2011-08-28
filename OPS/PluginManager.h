
#ifndef __PluginManager_H__
#define __PluginManager_H__

#pragma once
#include "Common/OgrePrerequisites.h"

#include "Common/OgreSingleton.h"
#include "Common/OgreString.h"

#include <exception>

namespace Ogre {

	class _OgreExport PluginManager : public Singleton<PluginManager>, public PluginAlloc
	{
	public:
        typedef vector<Plugin*>::type       PluginInstanceList;
        typedef vector<DynLib*>::type       PluginLibsList;
		typedef map<Plugin*, DynLib*>::type PluginsMap;
		typedef multimap<DynLib*, Plugin*>::type PluginLibsMap;
	protected:
        /// Список загруженных плагинов.
        PluginInstanceList  mPlugins;
        /// Список загруженных библиотек с плагинами.
        PluginLibsList      mPluginLibs;
        /// Отображение плагина на библиотеку, из которой он загружен.
        PluginsMap          mPluginsMap;
        /// Отображение библиотек на список их плагинов.
        PluginLibsMap       mPluginLibsMap;
        /// Последняя загруженная библиотека.
        DynLib*             mLastLoadLib;
	public:
		OGRE_AUTO_MUTEX // public to allow external locking

		PluginManager(const String& pluginFileName = "plugins.cfg");
		~PluginManager();

		
        /** Override standard Singleton retrieval.
        @remarks
            Why do we do this? Well, it's because the Singleton
            implementation is in a .h file, which means it gets compiled
            into anybody who includes it. This is needed for the
            Singleton template to work, but we actually only want it
            compiled into the implementation of the class based on the
            Singleton, not all of them. If we don't change this, we get
            link errors when trying to use the Singleton-based class from
            an outside dll.
        @par
            This method just delegates to the template version anyway,
            but the implementation stays in this single compilation unit,
            preventing link errors.
        */
        static PluginManager& getSingleton(void);
        /** Override standard Singleton retrieval.
        @remarks
            Why do we do this? Well, it's because the Singleton
            implementation is in a .h file, which means it gets compiled
            into anybody who includes it. This is needed for the
            Singleton template to work, but we actually only want it
            compiled into the implementation of the class based on the
            Singleton, not all of them. If we don't change this, we get
            link errors when trying to use the Singleton-based class from
            an outside dll.
        @par
            This method just delegates to the template version anyway,
            but the implementation stays in this single compilation unit,
            preventing link errors.
        */
        static PluginManager* getSingletonPtr(void);

		/** Method reads a plugins configuration file and instantiates all
			plugins.
		@param
			pluginsfile The file that contains plugins information.
			Defaults to "plugins.cfg".
		*/
        void loadPlugins(const String& pluginsfile = "plugins.cfg");
		/** Initialise all loaded plugins - allows plugins to perform actions
			once the renderer is initialised.
		*/
		void initialisePlugins();
		/** Shuts down all loaded plugins - allows things to be tidied up whilst
			all plugins are still loaded.
		*/
		void shutdownPlugins();
		/** Unloads all loaded plugins.
		*/
		void unloadPlugins();

		/** Manually load a Plugin contained in a DLL / DSO.
		@remarks
	 		Plugins embedded in DLLs can be loaded at startup using the plugin 
			configuration file specified when you create PluginManager (default: plugins.cfg).
			This method allows you to load plugin DLLs directly in code.
			The DLL in question is expected to implement a dllStartPlugin 
			method which instantiates a Plugin subclass and calls PluginManager::installPlugin.
			It should also implement dllStopPlugin (see PluginManager::unloadPlugin)
		@param pluginName Name of the plugin library to load
		*/
		void loadPlugin(const String& pluginName);

		/** Manually unloads a Plugin contained in a DLL / DSO.
		@remarks
	 		Plugin DLLs are unloaded at shutdown automatically. This method 
			allows you to unload plugins in code, but make sure their 
			dependencies are decoupled first. This method will call the 
			dllStopPlugin method defined in the DLL, which in turn should call
			Root::uninstallPlugin.
		@param pluginName Name of the plugin library to unload
		*/
		void unloadPlugin(const String& pluginName);

		/** Install a new plugin.
		@remarks
			This installs a new extension to OGRE. The plugin itself may be loaded
			from a DLL / DSO, or it might be statically linked into your own 
			application. Either way, something has to call this method to get
			it registered and functioning. You should only call this method directly
			if your plugin is not in a DLL that could otherwise be loaded with 
			loadPlugin, since the DLL function dllStartPlugin should call this
			method when the DLL is loaded. 
		*/
		void installPlugin(Plugin* plugin);

		/** Uninstall an existing plugin.
		@remarks
			This uninstalls an extension to OGRE. Plugins are automatically 
			uninstalled at shutdown but this lets you remove them early. 
			If the plugin was loaded from a DLL / DSO you should call unloadPlugin
			which should result in this method getting called anyway (if the DLL
			is well behaved).
		*/
		void uninstallPlugin(Plugin* plugin);

        DynLib* getLibrary(Plugin* plugin);
	};

}// namespace Ogre
#endif // __PluginManager_H__
