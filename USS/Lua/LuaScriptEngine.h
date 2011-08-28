#ifndef __LuaScriptEngine_H__
#define __LuaScriptEngine_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "USS/ScriptEngine.h"

#include <boost/pool/pool.hpp>

extern "C" {
struct lua_State;
struct luaL_Reg;
}
using namespace Script;

class LuaScriptEngine : public ScriptEngine
{
    /// Состояние lua для скриптового движка.
    lua_State *L;
    enum AllocatingType 
    { 
        GENERAL
      , SCRIPTABLE
    } type;
    boost::pool<> pool;

    struct Class
    {
        static luaL_Reg mt[];
        static int __index   (lua_State *L);
        static int __newindex(lua_State *L);
        static int __call    (lua_State *L);
        static int __GC      (lua_State *L);
    };
    struct Object
    {
        static luaL_Reg mt[];
        /** Вызывается при попытке получить доступ к полю объекта.
        @remarks
            Алгоритм:
            -# Найти @link IField поле@endlink с указанным именем. Если поле не найдено, перейти к шагу 4.
            -# Если поле является @link IProperty свойством@endlink, то вычислить его значение и выйти.
            -# Вернуть функцию-замыкание с информацией о том, метод с каким именем был вызван.
            -# Если объект индексируеый, то проиндексировать его. В случае успеха выйти.
            -# Перенаправить индексацию в метатаблицу (разрешено использование метаметодов).
        */
        static int __index   (lua_State *L);
        /// Вызывается при попытке записать в поле информацию.
        static int __newindex(lua_State *L);
        /** Метаметод для вычисления объекта как функции.
            Так как поля объекта тоже являются IObject-объектами,
            также вычисляет методы объекта.
        @notes
            Алгоритм:
            -# Если объект вызываемый, вызвать его и выйти.
            -# Перенаправить вызов в метатаблицу метатаблицы объекта.
        */
        static int __call    (lua_State *L);
        /// Вызывается при сборке IScriptable сборщиком мусора Lua.
        static int __GC      (lua_State *L);
    };

    class Compiler 
    {
        enum { READ_SIZE = 1024 };
        static char buf[READ_SIZE];
    public:
        struct Options {
            DataStream* s; 
            bool stripDebug;
        };
    public:
        /**
        @param L
            Состояние Lua.
        @param p
            Блок данных для записи.
        @param size
            Размер записываемого блока данных.
        @param u
            Данные, переданные в функцию lua_dump.
        */
        static int writer(lua_State* L, const void* p, size_t size, void* data);
        static const char* reader(lua_State* L, void* data, size_t *size);
        static int dumper(lua_State* L);
    };
    
public:
    LuaScriptEngine();
    virtual ~LuaScriptEngine();
    virtual void registerModule(IModule* module);
    virtual void unregisterModule(IModule* module);
    virtual void registerClass(IClass *clazz);
    virtual void unregisterClass(IClass *clazz);
    virtual bool registerObject(const String& name, IObject* object, bool bAllowDelete = true);
    virtual void unregisterObject(IObject* object);
    virtual StringVector extensions() const;
    virtual String version() const;
    virtual String getType() const;
    virtual ScriptVarList exec(DataStreamPtr script, const String& functionName, const ScriptVarList& args = ScriptVarList::BLANK);
    virtual bool compile(DataStreamPtr source, DataStreamPtr target, CompileOption options, bool stripDebug = false);
private:
    virtual void unregister();

private:
    static IMethod* findMethodTable(lua_State *L, const FieldList& fields, const String& name);
    static IConstructor* findConstructorTable(lua_State *L, const ConstructorList& constructors);
    static IEvaluator* findEvaluatorTable(lua_State *L, const EvaluatorList& evaluators);
    static void pushObject(lua_State* L, IObject* o);

    static IScriptable* getScriptable(lua_State *L, int index);
    static IObject* getObject(lua_State *L, int index);
    static IClass*  getClass (lua_State *L, int index);

private:
    /** Аллокатор памяти для Lua.
    @param ud 
        Указатель на LuaScriptEngine.
    @param ptr 
        Указатель на блок выделяемой или удаляемой памяти. Равен @c NULL тогда
        и только тогда, когда @a osize равен 0.
    @param osize 
        Исходный размер блока.
    @param nsize 
        Новый размер блока.
    @return
        Указатель на на блок [пере]выделенной памяти или @c NULL, если 
        [пере]выделить память не удалось или в случае освобождения памяти.
    */
    static void* allocateFunction(void *ud, void *ptr, size_t osize, size_t nsize);
    static int panicFunction(lua_State *L);
    static int errorHandler(lua_State *L);

    /** Вспомогательная функция для загрузки кода Lua из потока.
    */
    bool load(DataStreamPtr source);
    static int dumper(lua_State* L);

    /// Функция-замыкание, возвращаемое для методов. Таким образом методы можно
    /// использовать как обычные функции Lua.
    static int accessor(lua_State *L);

private:
    /// Метаметод для превращения IScriptable в строку.
    static int __tostring(lua_State *L);
};

#endif // __LuaScriptEngine_H__
