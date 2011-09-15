
#include "OLS/OgreLogManager.h"
#include "OAS/OgreArchiveManager.h"
#include "OAS/OgreFileSystem.h"
#include "ODLS/OgreDynLibManager.h"
#include "OPS/PluginManager.h"
#include "OES/OgreException.h"
#include "USS/ScriptManager.h"
#include "USS/ScriptVar.h"

#include <iostream>

using namespace std;
using namespace Ogre;
using namespace Script;

class Main
{
public:
    Main(const String& logName = "Test.log")
    {
        OGRE_NEW LogManager();
        Log* log = LogManager::getSingleton().createLog( logName );
        log->logMessage( "Starting LogManager..." );
        ArchiveManager* am = OGRE_NEW ArchiveManager();
        am->addArchiveFactory( new FileSystemArchiveFactory() );
        OGRE_NEW DynLibManager();
        OGRE_NEW PluginManager();
        OGRE_NEW ScriptManager();
    }
    ~Main()
    {
        PluginManager::getSingleton().shutdownPlugins();

        OGRE_DELETE ScriptManager::getSingletonPtr();
        OGRE_DELETE PluginManager::getSingletonPtr();
        OGRE_DELETE DynLibManager::getSingletonPtr();
        OGRE_DELETE ArchiveManager::getSingletonPtr();
        OGRE_DELETE LogManager::getSingletonPtr();
    }
    
    void loadPlugins(const String& pluginsDir = "plugins", const String& pattern = "*.dll")
    {
        Archive* archive = ArchiveManager::getSingleton().load( pluginsDir, "FileSystem" );
        
        StringVectorPtr dlls = archive->find( pattern, false );
        String dir = StringUtil::standardisePath( pluginsDir );
        for ( StringVector::const_iterator it = dlls->begin(); it != dlls->end(); ++it )
        {
            try {
                PluginManager::getSingleton().loadPlugin( dir + *it );
            } catch (const InternalErrorException&) {
                // ничего страшного - не нашли файл, ну и пусть
            }
        }
    }
};

int main(int argc, char* argv[])
{
    if ( argc < 2 )
    {
        cout << "Usage: loader filepattern [funname [funname ...]]\n\n"
                "filepattern - plugin pattern in 'plugins' directory.\n"
                "Allowed * and ? wildcards.";
        return 0;
    }

    Main app;
    ++argv;
    app.loadPlugins("./plugins", *argv);

    PluginManager::getSingleton().initialisePlugins();

    ScriptManager* sm = ScriptManager::getSingletonPtr();
    sm->setDefaultEngine("lua");
    //sm->exec("../../tests/demo");// Автоопределение по содержимому
    //sm->exec("../../tests/demo.lua");// Автоопределение по расширению
    //sm->exec("../../tests/demo2", "lua");// Исполниться как lua, незасисимо от содержимого

    ScriptVarListPtr inArgs(new ScriptVarList());
    inArgs->add( new StringVar("scriptName", "../../tests/demo2.lua") );
    inArgs->add( new IntVar("version", 1) );

    sm->exec("../../tests/demo2.lua", "simple", "lua", *inArgs);// Исполниться как lua, незасисимо от содержимого

    ++argv;
    while (*argv)
    {
        sm->exec("../../tests/demo2.lua", *argv, "lua", *inArgs);// Исполниться как lua, незасисимо от содержимого
        ++argv;
    }

    //sm->compile("../../tests/compile.lua", "../../tests/compile.luc", "lua", ScriptEngine::CO_BYTECODE);
    //sm->exec("../../tests/compile.luc", "lua");

    //sm->compile("../../tests/compile.lua", "../../tests/compile_nodebug.luc", "lua", ScriptEngine::CO_BYTECODE, true);
    //sm->exec("../../tests/compile_nodebug.luc", "lua");

    return 0;
}
