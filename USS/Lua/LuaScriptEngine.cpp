
#include "OLS/OgreLogManager.h"
#include "OLS/OgreLog.h"
#include "OES/OgreException.h"

#include "USS/ScriptVar.h"

#include "LuaScriptEngine.h"
#include "LuaBridge.h"
#include "LuaDebug.h"

#include "USS/IClass.h"
#include "USS/IObject.h"
#include "USS/IField.h"
#include "USS/IProperty.h"
#include "USS/IMethod.h"
#include "USS/IIndexator.h"
#include "USS/IConstructor.h"

#include <algorithm>

#define MAKE_VERSION_NUMBER(major, minor, revision, build)  #major "." #minor "." #revision "." #build
#define VERSION         MAKE_VERSION_NUMBER(0,0,5,0)

#define STR2LUA(x)       String(x).c_str()
#define PUSHSTR2LUA(x)   lua_pushlstring( L, STR2LUA(x), (x).size() )
#define RAISE_ERROR(msg) raiseError(L, BOOST_CURRENT_FUNCTION, msg)

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
// для sizeof(Udata)
#include "lobject.h"
}
#include "LuaUtilityClasses.h"

#include <boost/utility/addressof.hpp>
//conversion from 'size_t' to 'int', possible loss of data
#pragma warning(push)
#pragma warning(disable:4267)

LuaScriptEngine::LuaScriptEngine()
    : type( GENERAL )
    // размер объекта userdata + накладные расходы Lua
    , pool( sizeof(void*) + sizeof(Udata) )
{
    L = lua_newstate(LuaScriptEngine::allocateFunction, this);
    lua_atpanic(L, LuaScriptEngine::panicFunction);
    // регистрируем стандартные библиотеки
    luaL_openlibs(L);

    //TODO зарегистрировать объект ScriptEngine, предоставляющем информацию о движке
    //registerObject( "ScriptEngine", new ScriptEngineInfo(), false );
}
//------------------------------------------------------------------------
LuaScriptEngine::~LuaScriptEngine()
{
    lua_close(L);
    LOG("LuaScriptEngine engine destroyed.");
}

//========================================================================
//                          И Н Т Е Р Ф Е Й С
//========================================================================
void LuaScriptEngine::registerModule(IModule* module)
{
    //TODO
}
//------------------------------------------------------------------------
void LuaScriptEngine::unregisterModule(IModule* module)
{
    //TODO
}
//------------------------------------------------------------------------
void LuaScriptEngine::registerClass(IClass *clazz) 
{//FIXME при замене объекта метатаблица не поменяется
    assert( clazz );

    // Сохраняем указатель на фабрику в недрах lua-машины
    //LuaScriptEngine::pushScriptable( L, clazz );    // стек: [IScriptable]
    // стек: [] -> [IScriptable]
    *reinterpret_cast<const IClass**>(lua_newuserdata(L, sizeof(clazz))) = clazz;

    if ( luaL_newmetatable(L, ("USS_C:"+clazz->toString()).c_str()) )// стек: [ud mt]
    {// В новую таблицу записываем функции доступа к объектам
        
        luaL_register(L, NULL, Class::mt);
    }
    lua_setmetatable( L, -2 );                 // стек: [ud]
    lua_setglobal( L, STR2LUA(clazz->getName()) );  // стек: []
}
//------------------------------------------------------------------------
void LuaScriptEngine::unregisterClass(IClass* clazz)
{
    //TODO
}
//------------------------------------------------------------------------
bool LuaScriptEngine::registerObject(const String& name, IObject* o, bool bAllowDelete)
{
    //LuaScriptEngine::pushScriptable( L, o );// стек: [IScriptable]
    lua_setglobal( L, STR2LUA(name) );      // стек: []
    return true;
}
//------------------------------------------------------------------------
void LuaScriptEngine::unregisterObject(IObject* o)
{
    //TODO
}
//------------------------------------------------------------------------
StringVector LuaScriptEngine::extensions() const
{
    static StringVector exts;
    if ( exts.empty() )
    {
        exts.push_back("luc");
        exts.push_back("lua");
    }
    return exts;
}
//------------------------------------------------------------------------
String LuaScriptEngine::version() const
{
    return "LuaScriptEngine " VERSION " with " LUA_RELEASE;
}
//------------------------------------------------------------------------
String LuaScriptEngine::getType() const
{
    return "lua";
}
//------------------------------------------------------------------------
ScriptVarList LuaScriptEngine::exec(DataStreamPtr source, const String& functionName, const ScriptVarList& args)
{
    lua_pushcfunction(L, LuaScriptEngine::errorHandler);
    String scriptName = source->getName();

    ScriptVarList outArgs;
    if ( !load(source) )
        return outArgs;

    if ( !functionName.empty() )
    {
        // исполняем глобальную порцию для ввода функции.
        // К сожалению, тут могут быть побочные эффекты.
        if ( lua_pcall( L, 0, 0, 1 ) != 0 )
        {
            StringStream ss;
            ss << "Unexpected error while load script '" << scriptName << "'." << std::endl;
            ss << lua_tostring(L, -1);
            WARNING( ss.str() );
            return outArgs;
        }
        // функция для вызова
        lua_getfield( L, LUA_GLOBALSINDEX, STR2LUA(functionName) );
    }

    LuaBridge bridge(L);
    args.pack( bridge );
    if ( lua_pcall( L, args.size(), LUA_MULTRET, 1 ) == 0 )
    {   //TODO непонятно, что возвратит функция, поэтому мы не можем вернуть
        // результат. Надо узнать, что возвращает функция.
        outArgs.unpack( bridge );
        return outArgs;
    }
    StringStream ss;
    ss << "Unexpected error in ";
    functionName.empty() ? (ss << "<global chunk>") : (ss << "function '" << functionName << "'");
    ss << " in script " << scriptName << std::endl;
    if ( lua_isstring(L, -1) )
        ss << lua_tostring(L, -1);
    WARNING( ss.str() );
    return outArgs;
}
//------------------------------------------------------------------------
bool LuaScriptEngine::compile(DataStreamPtr source, DataStreamPtr target, ScriptEngine::CompileOption options, bool stripDebug)
{
    if ( options == ScriptEngine::CO_NONE )
    {
        target = source;
        return true;
    }
    if ( options == ScriptEngine::CO_BYTECODE )
    {
        Options opt(this, source, target, stripDebug);

        if ( lua_cpcall( L, LuaScriptEngine::dumper, &opt) != 0 )
        {
            String msg = lua_tostring(L, -1);
            OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, msg, "LuaScriptEngine::compile");
            return false;
        }
        return true;
    }
    return false;
}

void LuaScriptEngine::unregister()
{
    lua_close(L);
    L = lua_newstate(LuaScriptEngine::allocateFunction, this);
}
//========================================================================
//                    Д Е Т А Л И   Р Е А Л И З А Ц И И
//========================================================================

//------------------------------------------------------------------------
//--- Общие функции для взаимодействия с Lua -----------------------------
//------------------------------------------------------------------------
void* LuaScriptEngine::allocateFunction(void *ud, void *ptr, size_t osize, size_t nsize) 
{
    LuaScriptEngine* this_ = static_cast<LuaScriptEngine*>(ud);
    (void)osize;

    if ( this_->type == LuaScriptEngine::SCRIPTABLE )
    {// Выделение памяти из пула.
        if (nsize == 0) {
            assert( this_->pool.is_from(ptr) );
            this_->pool.free(ptr);
            return NULL;
        }
        assert( nsize == sizeof(void*) );
        return this_->pool.malloc();
    }
    if (nsize == 0) {
        free(ptr);
        return NULL;
    }
    else
        return realloc(ptr, nsize);
}
//------------------------------------------------------------------------
int LuaScriptEngine::panicFunction(lua_State *L)
{
    TRACE_IN;
    // String msg;
	// стек: [errmsg]
	lua_close(L);
	L = NULL;

	LOG("Lua: error occurred in unprotected call. This is very bad.");
	return 1;
}
//------------------------------------------------------------------------
int LuaScriptEngine::errorHandler(lua_State *L)
{
    // исходное сообщение
    //String msg = lua_tostring( L, -1 );
    
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_remove(L, -2);
    lua_pushvalue(L, 1);

    lua_call(L, 1, 1);
    // сообщение от Lua
    //String msg = lua_tostring( L, -1 );
    return 1;
}
//------------------------------------------------------------------------
bool LuaScriptEngine::load(DataStreamPtr source)
{
    // Собака указывает на то, что это имя файла, а не порция кода
    String scriptName = source->getName();
    if ( lua_load(L, LuaScriptEngine::Compiler::reader, source.get(), ('@'+scriptName).c_str()) != 0 )
    {
        if ( lua_isstring(L, -1) )
        {
            String err = lua_tostring(L, -1);
            WARNING( "Script '" + scriptName + "' not load:\n" + err );
        }
        else
            WARNING( "Script '" + scriptName + "' not load." );
        return false;
    }
    return true;
}
//------------------------------------------------------------------------
int LuaScriptEngine::dumper(lua_State* L)
{
    Options* opt = static_cast<Options*>(lua_touserdata(L, 1));

    if ( !opt->engine->load(opt->source) )
        return 0;

    //lua_dump(L, LuaScriptEnginePrivate::Compiler::writer, opt->target.get(), opt->stripDebug);

    return 0;
}
//------------------------------------------------------------------------
void LuaScriptEngine::pushObject(lua_State* L, IObject* o)
{
    assert(o);
    // стек: [] -> [IObject]
    *reinterpret_cast<const IObject**>(lua_newuserdata(L, sizeof(o))) = o;

    if ( luaL_newmetatable(L, ("USS_O:"+o->getClass()->toString()).c_str() ) )// стек: [IObject mt]
    {// В новую таблицу записываем функции доступа к объектам
        luaL_register(L, NULL, Object::mt);
    }
    lua_setmetatable( L, -2 );                 // стек: [IObject]
}
//------------------------------------------------------------------------
IScriptable* LuaScriptEngine::getScriptable(lua_State *L, int index)
{
    // стек: [] -> [IScriptable]
    void* p = lua_touserdata(L, index);
    if (!p)
        return NULL;
    //TODO Сделать проверку по адресу в памяти!
    return *static_cast<IScriptable**>(p);
}
//------------------------------------------------------------------------
IObject* LuaScriptEngine::getObject(lua_State *L, int index)
{
    // стек: [] -> [IObject]
    IObject* o = static_cast<IObject*>(getScriptable(L, index));
    luaL_argcheck(L, o, index, "expected IObject argument");

    return o;
}
//------------------------------------------------------------------------
IClass* LuaScriptEngine::getClass(lua_State *L, int index)
{
    // стек: [] -> [IClass]
    IClass* c = static_cast<IClass*>(getScriptable(L, index));
    luaL_argcheck(L, c, index, "expected IClass argument");

    return c;
}
//------------------------------------------------------------------------
int LuaScriptEngine::__tostring(lua_State *L)
{
    IScriptable* s = getScriptable(L, 1);
    if ( !s )
        return 0;// Не наш объект оставим в покое

    String str = s->toString();
    lua_pushlstring( L, str.c_str(), str.size() );
    return 1;
}
//------------------------------------------------------------------------
// Хелпер для вызова методов
int LuaScriptEngine::accessor(lua_State* L)
{
    IClass* c = reinterpret_cast<IClass*>(lua_touserdata(L, lua_upvalueindex(1)));
    String key = lua_tostring(L, lua_upvalueindex(2));
    //TODO Добавить проверку при получении объекта
    IObject* o = getObject(L, 1);

    luaL_argcheck(L, o->getClass() == c, 1, ("expected class "+c->toString() + ", got "+o->getClass()->toString()).c_str());

    // Ищем наиболее подходящий метод. В случае отсутствия такового передадим задачу
    // метатаблице.
    IMethod* m = findMethod( c->getMethods(), key, LuaBridge(L, 2) );
    ScriptVarListPtr inArgs;
    if ( !m )
    {
        m = findMethodTable( L, c->getFields(), key );
        if ( !m )
        {
            //luaL_
            return 0;
        }
        inArgs = m->inArgs();
        std::for_each( inArgs->begin(), inArgs->end(), TableUnpacker(L, -1) );
    } else {
        inArgs = m->inArgs();
        inArgs->unpack( LuaBridge(L, 2) );
    }

    ScriptVarListPtr outArgs = m->outArgs();

    m->eval( o, *inArgs, *outArgs );
    outArgs->pack( LuaBridge(L) );
    return outArgs->size();
}

//------------------------------------------------------------------------
// Находит наилучший метод, когда аргументы переданы в виде таблицы
IMethod* LuaScriptEngine::findMethodTable(lua_State *L, const FieldList& fields, const String& name)
{
    typedef std::pair<int, IMethod*> Result;
    const int t = -1;

    if ( !lua_istable(L, t) )
        return NULL;

    Result max(0, NULL);

    FieldList::const_iterator it = fields.find( &IFieldFinder(name) );
    
    // Пробегаем по всем методам и для каждого считаем оценку.
    // Метод с наилучшей оценкой возвращаем как результат.
    while ( (*it)->getName() == name )
    {
        IMethod* m = static_cast<IMethod*>(*it);

        ScriptVarListPtr inArgs = m->inArgs();

        TableChecker tc = std::for_each( inArgs->begin()
                                       , inArgs->end()
                                       , TableChecker(L, t) );
        if ( tc.estimate() > max.first || inArgs->empty() )
            max = std::make_pair( tc.estimate(), m );
    }
    return max.second;
}
IConstructor* LuaScriptEngine::findConstructorTable(lua_State *L, const ConstructorList& constructors)
{
    typedef std::pair<int, IConstructor*> Result;
    const int t = -1;

    if ( !lua_istable(L, t) )
        return NULL;

    Result max(0, NULL);

    ConstructorList::const_iterator it = constructors.begin();
    
    // Пробегаем по всем методам и для каждого считаем оценку.
    // Метод с наилучшей оценкой возвращаем как результат.
    for ( ; it != constructors.end(); ++it )
    {
        IConstructor* m = static_cast<IConstructor*>(*it);

        ScriptVarListPtr inArgs = m->inArgs();

        TableChecker tc = std::for_each( inArgs->begin()
                                       , inArgs->end()
                                       , TableChecker(L, t) );
        if ( tc.estimate() > max.first || inArgs->empty() )
            max = std::make_pair( tc.estimate(), m );
    }
    return max.second;
}
// Находит наилучший вычислитель, когда аргументы переданы в виде таблицы
IEvaluator* LuaScriptEngine::findEvaluatorTable(lua_State *L, const EvaluatorList& evaluators)
{
    typedef std::pair<int, IEvaluator*> Result;
    const int t = -1;

    if ( !lua_istable(L, t) )
        return NULL;

    EvaluatorList::const_iterator it = evaluators.begin();

    Result max(0, NULL);
    
    // Пробегаем по всем методам и для каждого считаем оценку.
    // Метод с наилучшей оценкой возвращаем как результат.
    for ( ; it != evaluators.end(); ++it )
    {
        IEvaluator* e = static_cast<IEvaluator*>(*it);

        ScriptVarListPtr inArgs = e->inArgs();

        TableChecker tc = std::for_each( inArgs->begin()
                                       , inArgs->end()
                                       , TableChecker(L, t) );
        if ( tc.estimate() > max.first || inArgs->empty() )
            max = std::make_pair( tc.estimate(), e );
    }
    return max.second;
}

//------------------------------------------------------------------------
//--- Class --------------------------------------------------------------
//------------------------------------------------------------------------
#include "LuaClassMetamethods.h"

//------------------------------------------------------------------------
//--- Object -------------------------------------------------------------
//------------------------------------------------------------------------
#include "LuaObjectMetamethods.h"

#pragma warning(pop)//C4267
