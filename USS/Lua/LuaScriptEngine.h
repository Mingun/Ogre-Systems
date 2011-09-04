#ifndef __LuaScriptEngine_H__
#define __LuaScriptEngine_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "USS/ScriptEngine.h"
#include <boost/pool/pool.hpp>

// Получаем смещеный на размер указателя указатель.
// Там расположен указатель на скриптовый движок.
#define engine(L) (*static_cast<LuaScriptEngine**>(\
    static_cast<void*>(static_cast<void**>(static_cast<void*>(L))-1)))

extern "C" {
struct lua_State;
}
using namespace Script;

class LuaScriptEngine : public ScriptEngine
{
    /// Состояние lua для скриптового движка.
    lua_State* L;
    enum AllocatingType 
    { 
        GENERAL
      , SCRIPTABLE
    } type;
    /** Пул памяти для размещения указателей на IScriptable.
    @remarks
        Необходимо вызывать деструкторы указателей тех объектов, которые
        больше не нужны. Возлагать эту задачу на метаметод __gc опасно,
        так как пользовательский код может удалить или заменить его. Поэтому
        деструктор будет вызываться при освобождении памяти. Однако функции
        выделения нельзя сообщить, что освобождаемая память принадлежит объекту,
        с помощью флага, как это делалось при выделении памяти. Единственный
        способ это сделасть - по освобождаемому указателю. Если он выделен из
        пула, эначит это наш скриптовый объект, в противном случае - какие-то
        иные данные.
    */
    boost::pool<> mUserObjects;
    bool mShutdown;

    struct Scriptable
    {
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
            Так как поля объекта тоже являются IScriptable-объектами,
            также вычисляет методы объекта.
        @notes
            Алгоритм:
            -# Если объект вызываемый, вызвать его и выйти.
            -# Перенаправить вызов в метатаблицу метатаблицы объекта.
        */
        static int __call    (lua_State *L);
        /// Метаметод для превращения IScriptable в строку.
        static int __tostring(lua_State *L);
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
    
    friend class LuaBridge;
public:
    LuaScriptEngine();
    virtual ~LuaScriptEngine();
    virtual void registerModule(IModulePtr module);
    virtual void unregisterModule(IModulePtr module);
    virtual void registerClass(IClassPtr clazz);
    virtual void unregisterClass(IClassPtr clazz);
    virtual bool registerObject(const String& name, IScriptablePtr object, bool bAllowDelete = true);
    virtual void unregisterObject(IScriptablePtr object);
    virtual StringVector extensions() const;
    virtual String version() const;
    virtual String getType() const;
    virtual ScriptVarList exec(DataStreamPtr script, const String& functionName, const ScriptVarList& args = ScriptVarList::BLANK);
    virtual bool compile(DataStreamPtr source, DataStreamPtr target, CompileOption options, bool stripDebug = false);

public:
    /// @return @c true, если был сделан запрос на shutdown, @c false иначе.
    inline bool isShutdown() const { return mShutdown; }

private:
    virtual void init();
    virtual void shutdown();
    /** Вставляет объект в стек и присваивает ему метатаблицу, регистрируя в
        ней функции, необходимые для его работы.
    @param L
        Стек Lua.
    @param object
        Помещаемый в стек объект. Объект должен валидным, иначе в отладке
        сработает assert.
    */
    void pushObject(const IScriptablePtr& object);

private:
    static IMethod* findMethodTable(lua_State *L, const FieldList& fields, const String& name);
    static IEvaluator* findEvaluatorTable(lua_State *L, const EvaluatorList& evaluators);

    /// Преобразует  userdata по данному индексу в указатель на IScriptable
    static IScriptablePtr getScriptable(lua_State *L, int index);

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
};

#endif // __LuaScriptEngine_H__
