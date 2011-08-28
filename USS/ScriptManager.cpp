
#include "OLS/OgreLogManager.h"
#include "OES/OgreException.h"
#include "USS/ScriptManager.h"
#include "USS/ScriptEngine.h"
#include "USS/IClass.h"
#include "USS/IObject.h"
#include "USS/IModule.h"

namespace Ogre {
    template<> Script::ScriptManager* Singleton<Script::ScriptManager>::ms_Singleton = 0;
}

namespace Script {

    //-----------------------------------------------------------------------
    //template<> ScriptManager* ::Ogre::Singleton<ScriptManager>::ms_Singleton = 0;
    ScriptManager* ScriptManager::getSingletonPtr(void)
    {
        return ms_Singleton;
    }
    ScriptManager& ScriptManager::getSingleton(void)
    {
        assert( ms_Singleton );  return ( *ms_Singleton );
    }
    //-----------------------------------------------------------------------
    ScriptManager::ScriptManager()
        : mMapExtensionsPolicy( MEP_REPLACE_ALL )
    {
        LOG("ScriptManager Initialising");
    }
    //-----------------------------------------------------------------------
    ScriptManager::~ScriptManager()
    {
        OGRE_LOCK_AUTO_MUTEX
        // Порядок удаления важен: сначала скриптовые движки, потом классы.

        for( ScriptEngineList::iterator it = mScriptEngines.begin(); it != mScriptEngines.end(); ++it )
        {
            it->second->unregister();
        }
        mScriptEngines.clear();
        mClasses.clear();
        LOG("ScriptManager Shutdown");
    }
    //-----------------------------------------------------------------------
    void ScriptManager::registerModule(IModulePtr module)
    {
        OGRE_LOCK_AUTO_MUTEX
        // Уведомляем все скриптовые движки о регистрации нового модуля
        ScriptEngineList::iterator it;
        for ( it = mScriptEngines.begin(); it != mScriptEngines.end(); ++it )
        {
            it->second->registerModule( module.get() );
        }
    }
    //-----------------------------------------------------------------------
    void ScriptManager::registerClass(IClassPtr clazz)
    {
        OGRE_LOCK_AUTO_MUTEX
        String name = clazz->getName();
        ClassList::iterator it = mClasses.find(name);
        if ( it != mClasses.end() )
        {
            WARNING("Scriptable class named " + name + " already exist and will replace.");
            //WARNING(tr("Scriptable factory named %1 already exist and will replace.").arg(name)
            //		 , "ScriptManager::registerObjectFactory");
        }
        mClasses.insert( ClassList::value_type(name, clazz) );
        //LOG( "Scriptable class %1 registered." );
        LOG( "Scriptable class " + name + " registered." );
        {
            // Уведомляем все скриптовые движки о регистрации новой фабрики объектов
            ScriptEngineList::iterator it;
            for ( it = mScriptEngines.begin(); it != mScriptEngines.end(); ++it )
            {
                it->second->registerClass( clazz.get() );
            }
        }
    }
    //-----------------------------------------------------------------------
    void ScriptManager::unregisterClass(const String& name)
    {
        OGRE_LOCK_AUTO_MUTEX
        ClassList::iterator it = mClasses.find(name);
        if ( it == mClasses.end() )
        {
            WARNING("Scriptable class named " + name + " not found.");
            //WARNING(tr("Scriptable class named %1 not found.").arg(name)
            //		 , "ScriptManager::unregisterObjectFactory");
            return;
        }
        mClasses.erase( it );
        //LOG( "Scriptable class %1 unregistered." );
        LOG( "Scriptable class " + name + " unregistered." );
        /*{
            // Уведомляем все скриптовые движки о дерегистрации новой фабрики объектов
            ScriptEngineList::iterator it;
            for ( it = mScriptEngines.begin(); it != mScriptEngines.end(); ++it )
            {
                i->second->unregisterClass( factory );
            }
        }*/
    }

    //-----------------------------------------------------------------------
    IClassPtr ScriptManager::getClass(const String& name)
    {
        OGRE_LOCK_AUTO_MUTEX
        ClassList::iterator it = mClasses.find(name);
        if ( it == mClasses.end() )
        {
            WARNING("Scriptable class named " + name + " not found.");
            return IClassPtr();
        }
        return it->second;
    }
    //-----------------------------------------------------------------------
    void ScriptManager::registerObject(const String& name, IObjectPtr o, bool bAllowDelete)
    {
        OGRE_LOCK_AUTO_MUTEX
        {
            ObjectList::const_iterator it = mObjects.find(name);
            if ( it != mObjects.end() )
            {
                WARNING("Scriptable object named '" + name + 
                        "' already exist and will replace.");
                //TODO удаляем предыдущий объект
                /*ScriptEngineList::iterator seIt = mScriptEngines.begin();
                for ( ; seIt != mScriptEngines.end(); ++seIt )
                {
                    seIt->second->unregisterObject( name );
                }*/
            }
            mObjects.insert( ObjectList::value_type(name, o) );
        }
        ScriptEngineList::iterator it = mScriptEngines.begin();
        for ( ; it != mScriptEngines.end(); ++it )
        {
            it->second->registerObject( name, o.get(), bAllowDelete );
        }
    }
    //-----------------------------------------------------------------------
    IObjectPtr ScriptManager::getObject(const String& name)
    {
        //OGRE_LOCK_AUTO_MUTEX
        OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "", "ScriptManager::getObject");
        return IObjectPtr();
    }
    //-----------------------------------------------------------------------
    void ScriptManager::registerScriptEngine(ScriptEnginePtr engine)
    {
        OGRE_LOCK_AUTO_MUTEX
        String name = engine->getType();
        ScriptEngineList::const_iterator it = mScriptEngines.find(name);
        if ( it != mScriptEngines.end() )
        {
            WARNING("Script engine named '" + name + 
                    "' already exist and will replace.");
            //WARNING(tr("Script engine named %1 already exist and will replace.").arg(name)
            //		 , "ScriptManager::registerScriptEngine");
        }
        mScriptEngines.insert( ScriptEngineList::value_type(name, engine) );
        // Регистрируемся в списках расширений.
        StringVector exts = engine->extensions();
        if ( mMapExtensionsPolicy != MEP_DO_NOTHING )
        {
            for ( StringVector::const_iterator it = exts.begin(); it != exts.end(); ++it )
            {
                MapExtensionsList::const_iterator i = mMapExtensions.find(*it);
                if ( i != mMapExtensions.end() ) 
                {
                    if ( mMapExtensionsPolicy == MEP_REPLACE_ALL )
                        LOG("Conflict extension '" + *it + "', already used by '" + 
                            i->second->getType() + "' script engine. Now used '" +
                            engine->getType() + "' script engine.");
                    else
                    {
                        LOG("Conflict extension '" + *it + "', already used by '" + 
                            i->second->getType() + "' script engine. No change.");
                        continue;
                    }
                }
                mMapExtensions.insert( MapExtensionsList::value_type(*it, engine) );
            }
        }
        //LOG( "Script engine %1 registered." );
        LOG( "Script engine " + name + " registered." );
        {
            // Уведомляем новый скриптовый движок о фабриках объектов
            ClassList::iterator it;
            for ( it = mClasses.begin(); it != mClasses.end(); ++it )
            {
                engine->registerClass( it->second.get() );
            }
        }
        {
            // Уведомляем новый скриптовый движок об объектах
            ObjectList::iterator it;
            for ( it = mObjects.begin(); it != mObjects.end(); ++it )
            {
                engine->registerObject( it->first, it->second.get() );
            }
        }
    }
    //-----------------------------------------------------------------------
    void ScriptManager::unregisterScriptEngine(ScriptEnginePtr engine)
    {
        OGRE_LOCK_AUTO_MUTEX
        String name = engine->getType();
        ScriptEngineList::iterator it = mScriptEngines.find(name);
        if ( it == mScriptEngines.end() )
        {
            WARNING("Script engine named " + name + " not found.");
            //WARNING(tr("Script engine named %1 not found.").arg(name)
            //		 , "ScriptManager::unregisterScriptEngine");
            return;
        }
        it->second->unregister();
        mScriptEngines.erase( it );
        //LOG( "Script engine %1 unregistered." );
        LOG( "Script engine " + name + " unregistered." );
    }
    //-----------------------------------------------------------------------
    ScriptEnginePtr ScriptManager::getScriptEngine(const String& name)
    {
        OGRE_LOCK_AUTO_MUTEX
        ScriptEngineList::iterator it = mScriptEngines.find(name);
        if ( it == mScriptEngines.end() )
        {
            WARNING("Script engine named " + name + " not found.");
            return ScriptEnginePtr();
        }
        return it->second;
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(const String& scriptName, const ScriptVarList& args)
    {//TODO Сделать автоопределение по содержимому файла.
        OGRE_LOCK_AUTO_MUTEX

        String baseName, ext, path;
        StringUtil::splitFullFilename( scriptName, baseName, ext, path );

        if ( ext.empty() )
            ext = mDefaultEngineName;
        MapExtensionsList::iterator it = mMapExtensions.find( ext );
        if ( it == mMapExtensions.end() )
        {
            WARNING("Script engine for extension " + ext + " not found. Script '" +
                    scriptName + "' not run.");
            return ScriptVarList();
        }
        return exec( scriptName, StringUtil::BLANK, it->second->getType(), args );
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(const String& scriptName, const String& type, const ScriptVarList& args)
    {
        OGRE_LOCK_AUTO_MUTEX
        return exec( scriptName, StringUtil::BLANK, type, args );
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(const String& scriptName, const String& functionName, const String& type, const ScriptVarList& args)
    {
        OGRE_LOCK_AUTO_MUTEX
        
        std::ifstream is(scriptName.c_str(), std::ios_base::binary);
        if ( is.fail() )
        {
            WARNING("Script '" + scriptName + "' not found.");
            return ScriptVarList();
        }

        return exec( DataStreamPtr(new FileStreamDataStream(scriptName, &is, false)), functionName, type, args );
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(DataStreamPtr script, const ScriptVarList& args)
    {
        OGRE_LOCK_AUTO_MUTEX

        const String& fileName = script->getName();
        String baseName, ext, path;
        StringUtil::splitFullFilename( fileName, baseName, ext, path );

        if ( ext.empty() )
            ext = mDefaultEngineName;
        MapExtensionsList::iterator it = mMapExtensions.find( ext );
        if ( it == mMapExtensions.end() )
        {
            WARNING("Script engine for extension " + ext + " not found. Script '" +
                    fileName + "' not run.");
            return ScriptVarList();
        }
        return exec( script, StringUtil::BLANK, it->second->getType(), args ); 
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(DataStreamPtr script, const String& type, const ScriptVarList& args)
    {
        OGRE_LOCK_AUTO_MUTEX
        return exec( script, StringUtil::BLANK, type, args ); 
    }
    //-----------------------------------------------------------------------
    ScriptVarList ScriptManager::exec(DataStreamPtr script, const String& functionName, const String& type, const ScriptVarList& args)
    {
        OGRE_LOCK_AUTO_MUTEX
        ScriptEngineList::iterator it = mScriptEngines.find(type);
        if ( it == mScriptEngines.end() )
        {
            WARNING("Script engine named " + type + " not found.");
            //WARNING(tr("Script engine named %1 not found.").arg(type)
            //		 , "ScriptManager::exec");
            return ScriptVarList();
        }
        return it->second->exec( script, functionName, args );
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(DataStreamPtr source, DataStreamPtr target, bool stripDebug)
    {
        // Если не получилось скомпилировать в машинный код, пытаемся в байткод.
        // Если не получится в байткод, то просто возвращаем исходный поток.
        if ( !compile(source, target, ScriptEngine::CO_JIT) )
            if ( !compile(source, target, ScriptEngine::CO_BYTECODE, stripDebug) )
                target = source;
        return true;
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(DataStreamPtr source, DataStreamPtr target, ScriptEngine::CompileOption options, bool stripDebug)
    {
        OGRE_LOCK_AUTO_MUTEX

        ScriptEngineList::iterator it = mScriptEngines.begin();
        for ( ; it != mScriptEngines.end(); ++it )
        {
            //TODO Не все потоки могут поддерживать операцию seek...
            try {
                target->seek(0);
                if ( it->second->compile( source, target, options, stripDebug ) )
                    return true;
            } catch (const UnsupportedFeatureException&) {
            } catch (const InvalidParametersException&) {}
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(DataStreamPtr source, DataStreamPtr target, const String& type, ScriptEngine::CompileOption options, bool stripDebug)
    {
        OGRE_LOCK_AUTO_MUTEX
        ScriptEngineList::iterator it = mScriptEngines.find(type);
        if ( it == mScriptEngines.end() )
        {
            WARNING("Script engine named " + type + " not found.");
            return false;
        }
        return it->second->compile( source, target, options, stripDebug );
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(const String& sourceName, const String& targetName, bool stripDebug)
    {
        std::ifstream is(sourceName.c_str(), std::ios_base::binary);
        if ( is.fail() )
        {
            WARNING("Script '" + sourceName + "' not found.");
            return false;
        }
        std::fstream  os(targetName.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        if ( os.fail() )
        {
            WARNING("Cann't open file '" + targetName + "' for writing.");
            return false;
        }
        return compile( DataStreamPtr(new FileStreamDataStream(sourceName, &is, false))
                      , DataStreamPtr(new FileStreamDataStream(targetName, &os, false)), stripDebug );
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(const String& sourceName, const String& targetName, ScriptEngine::CompileOption options, bool stripDebug)
    {
        std::ifstream is(sourceName.c_str(), std::ios_base::binary);
        if ( is.fail() )
        {
            WARNING("Script '" + sourceName + "' not found.");
            return false;
        }
        std::fstream  os(targetName.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        if ( os.fail() )
        {
            WARNING("Cann't open file '" + targetName + "' for writing.");
            return false;
        }
        return compile( DataStreamPtr(new FileStreamDataStream(sourceName, &is, false))
                      , DataStreamPtr(new FileStreamDataStream(targetName, &os, false))
                      , options, stripDebug );
    }
    //-----------------------------------------------------------------------
    bool ScriptManager::compile(const String& sourceName, const String& targetName, const String& type, ScriptEngine::CompileOption options, bool stripDebug)
    {
        std::ifstream is(sourceName.c_str(), std::ios_base::binary);
        if ( is.fail() )
        {
            WARNING("Script '" + sourceName + "' not found.");
            return false;
        }
        std::fstream  os(targetName.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        if ( os.fail() )
        {
            WARNING("Cann't open file '" + targetName + "' for writing.");
            return false;
        }
        return compile( DataStreamPtr(new FileStreamDataStream(sourceName, &is, false))
                      , DataStreamPtr(new FileStreamDataStream(targetName, &os, false))
                      , type, options, stripDebug );
    }
    //-----------------------------------------------------------------------
    void ScriptManager::setDefaultEngine(const String& type)
    {
        ScriptEngineList::iterator it = mScriptEngines.find(type);
        if ( it == mScriptEngines.end() )
        {
            WARNING("Script engine named " + type + " not found.");
        }
        LOG("Default script engine changed to '" + type + "'.");
        mDefaultEngineName = type;
    }
    //-----------------------------------------------------------------------
    void ScriptManager::setMapExtensionsPolicy(MapExtensionsPolicy policy)
    {
        OGRE_LOCK_AUTO_MUTEX
        mMapExtensionsPolicy = policy;
    }
    //-----------------------------------------------------------------------
    MapExtensionsPolicy ScriptManager::getMapExtensionsPolicy() const
    {
        return mMapExtensionsPolicy;
    }
}// namespace Script;