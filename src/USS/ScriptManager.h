#ifndef __USSScriptManager_H__
#define __USSScriptManager_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreSingleton.h"
#include "Common/OgreString.h"

#include "OLS/OgreLog.h"
#include "ORS/OgreDataStream.h"
#include "USS/ScriptVarList.h"
#include "USS/ScriptEngine.h"

namespace Script {

    /** Перечисление политик отображения расширений при регистрации 
        нового движка.
    */
    enum MapExtensionsPolicy {
        /// Движок не будет регистрироваться ни для каких
        /// расширений. Это надо будет сделать вручную.
        MEP_DO_NOTHING
        /// Регистрация каждого нового движка будет переписывать 
        /// ассоцииации расширений под него в случае конфликтов.
        /// Это режим по умолчанию.
      , MEP_REPLACE_ALL
        /// Регистрация каждого нового движка НЕ будет переписывать 
        /// ассоцииации расширений под него в случае конфликтов,
        /// но будет регистрировать движок для свободных расширений.
      , MEP_REPLACE_ONLY_NEW
    };

/** Менеджер скриптов.
*/
class _ScriptExport ScriptManager : public Singleton<ScriptManager>, public ScriptableAlloc
{
protected:
    typedef map<String, IClassPtr>::type       ClassList;
    typedef map<String, IScriptablePtr>::type  ObjectList;
    typedef map<String, ScriptEnginePtr>::type ScriptEngineList;
    typedef map<String, ScriptEnginePtr>::type MapExtensionsList;

    ClassList           mClasses;
    ObjectList          mObjects;
    ScriptEngineList    mScriptEngines;
    /// Отображает расширение файла на скриптовый движок.
    MapExtensionsList   mMapExtensions;
    /// Тип скриптового движока для файлов скриптов без расширения.
    String              mDefaultEngineName;
    /// Политика ассоциации регистрируемых движков с расширениями
    /// файлов скриптов.
    MapExtensionsPolicy mMapExtensionsPolicy;

public:
    OGRE_AUTO_MUTEX // public to allow external locking

    ScriptManager();
    ~ScriptManager();

    void registerModule(IModulePtr module);
    //void unregisterModule(const String& name);
    //IModule* getModule(const String& name);

    void registerClass(IClassPtr clazz);
    void unregisterClass(const String& name);
    IClassPtr getClass(const String& name);

    void registerObject(const String& name, IScriptablePtr object, bool bAllowDelete = true);
    void unregisterObject(const String& name);
    IScriptablePtr getObject(const String& name);

    void registerScriptEngine(ScriptEnginePtr engine);
    void unregisterScriptEngine(ScriptEnginePtr engine);
    ScriptEnginePtr getScriptEngine(const String& name);

    ScriptVarList exec(const String& scriptName, const ScriptVarList& args = ScriptVarList::BLANK);
    ScriptVarList exec(const String& scriptName, const String& type, const ScriptVarList& args = ScriptVarList::BLANK);
    ScriptVarList exec(const String& scriptName, const String& functionName, const String& type, const ScriptVarList& args = ScriptVarList::BLANK);
    ScriptVarList exec(DataStreamPtr script, const ScriptVarList& args = ScriptVarList::BLANK);
    ScriptVarList exec(DataStreamPtr script, const String& type, const ScriptVarList& args = ScriptVarList::BLANK);
    /** Исполняет указанный поток как скрипт типа @c type.
    */
    ScriptVarList exec(DataStreamPtr script, const String& functionName, const String& type, const ScriptVarList& args = ScriptVarList::BLANK);

    /** Компилирует указанный поток данных. 
        Автоматически определяет тип скрипта и пытается скомпилировать его
        сначала в машинный код, если это не получится, компилирует в байткод,
        если и это не поддерживается, возвращает поток как есть.
    @param source
        Поток данных с компилируемым исходником скрипта.

    @return
        Поток данных со скомпилированным скриптом.
    */
    bool compile(DataStreamPtr source, DataStreamPtr target, bool stripDebug = false);

    /** Компилирует указанный поток данных.
        Автоматически определяет тип скрипта.
    @param source
        Поток данных с компилируемым исходником скрипта.
    @param target
        Тип данных выходного потока.

    @return
        Поток данных со скомпилированным скриптом.
    */
    bool compile(DataStreamPtr source, DataStreamPtr target, ScriptEngine::CompileOption options, bool stripDebug = false);

    /** Компилирует указанный поток данных.
    @param source
        Поток данных с компилируемым исходником скрипта.
    @param type
        Тип скриптового движка, используемый для компиляции.
    @param target
        Тип данных выходного потока.

    @return
        Поток данных со скомпилированным скриптом.
    */
    bool compile(DataStreamPtr source, DataStreamPtr target, const String& type, ScriptEngine::CompileOption options, bool stripDebug = false);

    bool compile(const String& sourceName, const String& targetName, bool stripDebug = false);
    bool compile(const String& sourceName, const String& targetName, ScriptEngine::CompileOption options, bool stripDebug = false);
    bool compile(const String& sourceName, const String& targetName, const String& type, ScriptEngine::CompileOption options, bool stripDebug = false);

    void setDefaultEngine(const String& type);
    void setMapExtensionsPolicy(MapExtensionsPolicy policy);
    MapExtensionsPolicy getMapExtensionsPolicy() const;

public:
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
    static ScriptManager& getSingleton(void);
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
    static ScriptManager* getSingletonPtr(void);

};

}// namespace Script

#endif // __USSScriptManager_H__
