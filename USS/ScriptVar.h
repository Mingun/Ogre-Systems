#ifndef __ScriptVar_H__
#define __ScriptVar_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "Common/JavaIterator.h"
#include "OMS/OgreSharedPtr.h"
#include "USS/Function.h"
#include "USS/Bridge.h"

#define USS_TYPEID \
protected:\
    friend class ScriptVar;\
    virtual inline bool is(const ScriptVar* to) const { return false; }\
    virtual inline bool is(const _Self* to) const { return true; }

#define RegisterVar(cls, type, initial) \
class _ScriptExport cls ## Var : public BaseVar<type> \
{\
    typedef BaseVar<type>    _Base;\
    typedef cls ## Var       _Self;\
private:\
    bool    mDefault;\
    cls ## Var(const String& name, type value, bool def) \
        : _Base( name, value ), mDefault( def ) {}\
public:\
    explicit cls ## Var(const String& name = StringUtil::BLANK) \
        : _Base( name, initial ), mDefault( false ) {}\
    explicit cls ## Var(type value) \
        : _Base( value ), mDefault( true ) {}\
    cls ## Var(const String& name, type value) \
        : _Base( name, value ), mDefault( true ) {}\
    virtual ScriptVar* clone() const { return new _Self(mName, mValue, mDefault); }\
    virtual String toString() const;\
    virtual bool hasDefault() const { return mDefault; }\
\
    USS_TYPEID\
};

namespace Script {

class _ScriptExport ScriptVar
{
public:
    virtual ~ScriptVar() {}
    virtual String toString() const = 0;
    virtual ScriptVar* clone() const = 0;

    /// Помещает значение переменной в скриптовые структуры.
    virtual void pack(Bridge& bridge) const = 0;
    /// Извлекает значение переменной из скриптовых структур.
    virtual void unpack(Bridge& bridge) = 0;
    virtual int check(Bridge& bridge) const = 0;

    template<class T>
    T* to() { return this->is( &T() ) ? static_cast<T*>(this) : NULL; }
    template<class T>
    const T* to() const { return this->is( &T() ) ? static_cast<const T*>(this) : NULL; }
    
    /// Получает имя типа переменной.
    virtual const String& typeName() const = 0;
    /// Получает имя переменной.
    virtual const String& name() const = 0;
    /// Возвращает @c true, если переменная имеет значение по умолчанию.
    virtual bool hasDefault() const = 0;
protected:
    // Перенаправляющие функции
    virtual bool is(const ScriptVar* to) const;
    virtual bool is(const BoolVar* to) const;
    virtual bool is(const IntVar* to) const;
    virtual bool is(const RealVar* to) const;
    virtual bool is(const CharVar* to) const;
    virtual bool is(const StringVar* to) const;
    virtual bool is(const IteratorVar* to) const;
    virtual bool is(const ScriptableVar* to) const;
    virtual bool is(const HashVar* to) const;
    virtual bool is(const FunctionVar* to) const;
};

template<typename T>
class BaseVar : public ScriptVar 
{
    typedef ScriptVar       _Base;
    typedef BaseVar<T>      _Self;
public:
    typedef T value_type;
protected:
    T       mValue;
    String  mName;
public:
    explicit BaseVar(T value) 
        : mValue( value ) {}
    BaseVar(const String& name, T value) 
        : mValue( value ), mName( name ) {}

    inline T& value() { return mValue; }
    inline const T& value() const { return mValue; }

    virtual void pack(Bridge& bridge) const { bridge.pack(mValue); }
    virtual void unpack(Bridge& bridge) { bridge.unpack(mValue); }
    virtual int check(Bridge& bridge) const { return bridge.check(mValue); }

    virtual const String& typeName() const {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
        static String type(typeid(T).name());
#else// mingw почему-то не хочет получать информацию о (пока) неизвестном типе
        static String type("BaseVar<T>");
#endif
        return type;
    }
    virtual const String& name() const { return mName; }
};
// Специализация для указателей
template<typename T>
class BaseVar<T*> : public ScriptVar
{
    typedef ScriptVar   _Base;
    typedef BaseVar<T*> _Self;
public:
    typedef T* value_type;
protected:
    T*      mValue;
    String  mName;
public:
    explicit BaseVar(T* value) 
        : mValue( value ) {}
    BaseVar(const String& name, T* value) 
        : mValue( value ), mName( name ) {}

    inline T*& value() { return mValue; }
    inline T* const& value() const { return mValue; }

    virtual void pack(Bridge& bridge) const { bridge.pack( mValue ); }
    virtual void unpack(Bridge& bridge) { bridge.unpack( mValue ); }
    virtual int check(Bridge& bridge) const { return bridge.check( mValue ); }
    virtual const String& typeName() const {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
        static String type(typeid(T).name());
#else// mingw почему-то не хочет получать информацию о (пока) неизвестном типе
        static String type("BaseVar<T*>");
#endif
        return type;
    }
    virtual const String& name() const { return mName; }
};
template<typename E>
class BaseIteratorVar : public BaseVar< java::Iterator<E>*> 
{
    typedef BaseVar<java::Iterator<E>*> _Base;
    typedef BaseIteratorVar             _Self;
private:
    bool    mDefault;
public:
    explicit BaseIteratorVar(const String& name = StringUtil::BLANK) 
        : _Base( name, NULL ), mDefault( false ) {}
    explicit BaseIteratorVar(const java::Iterator<E>* value) 
        : _Base( value ? value->clone() : NULL ), mDefault( true ) {}
    BaseIteratorVar(const String& name, const java::Iterator<E>* value) 
        : _Base( name, value ? value->clone() : NULL ), mDefault( true ) {}
    virtual ~BaseIteratorVar() { delete this->mValue; }
    virtual ScriptVar* clone() const { return new _Self(this->mName, this->mValue ? this->mValue->clone() : NULL); }

    virtual String toString() const
    {
        StringStream ss;
        ss << this->mValue;
        return ss.str();
    }
    virtual bool hasDefault() const { return this->mDefault; }
};

//========================================================================
RegisterVar(Bool, bool, false)
RegisterVar(Int,  int, 0)
RegisterVar(Real, double, 0.0)
RegisterVar(Char, char, 0)
RegisterVar(Scriptable, IScriptable*, NULL)
RegisterVar(Iterator, Iterator*, NULL)

class _ScriptExport StringVar : public BaseVar<String> 
{
    typedef BaseVar<String>  _Base;
    typedef StringVar        _Self;
private:
    bool    mDefault;
public:
    explicit StringVar(const String& name = StringUtil::BLANK) 
        : _Base( name, StringUtil::BLANK ), mDefault( false ) {}
    explicit StringVar(const String& name, const String& value) 
        : _Base( name, value ), mDefault( true ) {}
    virtual ScriptVar* clone() const { return new _Self(mName, mValue); }
    virtual String toString() const;
    virtual bool hasDefault() const { return this->mDefault; }

    USS_TYPEID
};
/*
class _ScriptExport IteratorVar : public BaseIteratorVar<ScriptVarList> 
{
    typedef BaseIteratorVar<ScriptVarList>  _Base;
    typedef IteratorVar                     _Self;
public:
    explicit IteratorVar(const Iterator* value = NULL) 
        : _Base( value ) {}
    IteratorVar(const String& name, const Iterator* value = NULL) 
        : _Base( name, value ) {}
    virtual ScriptVar* clone() const { return new _Self(mName, mValue ? mValue->clone() : NULL); }

    virtual void pack(Bridge* bridge) const { bridge->pack( mValue ); }
    virtual void unpack(Bridge* bridge) { bridge->unpack( mValue ); }
    virtual bool check(Bridge* bridge) const { return bridge->check( mValue ); }

    virtual String toString() const;

    USS_TYPEID
};*/

//-------------------------------- Хеш -----------------------------------
class _ScriptExport Hash : public HashMap<String, ScriptVar*>
{
    typedef HashMap<String, ScriptVar*> _Base;
    typedef Hash                        _Self;
public:
    Hash() : _Base() {}
    Hash(const Hash& rhs);
    ~Hash();

    inline void add(const String& key, ScriptVar* value) {
        insert( _Base::value_type(key, value) );
    }
    Hash* clone() const { return new _Self(*this); }
};

class _ScriptExport HashVar : public BaseVar<Hash>
{
    typedef BaseVar<Hash>   _Base;
    typedef HashVar         _Self;
public:
    explicit HashVar(const Hash& value) 
        : _Base( value ) {}
    HashVar(const String& name, const Hash& value)
        : _Base( name, value ) {}
    virtual ScriptVar* clone() const { return new _Self(mName, mValue); }
    virtual String toString() const;
    virtual bool hasDefault() const { return false; }

    USS_TYPEID
};

/** Переменная для хранения ссылок на функции.
*/
class _ScriptExport FunctionVar : public BaseVar<Function*>
{
    typedef BaseVar<Function*> _Base;
    typedef FunctionVar        _Self;
private:
    bool    mDefault;
public:
    explicit FunctionVar(const String& name = StringUtil::BLANK)
        : _Base( name, NULL ), mDefault( false ) {}
    explicit FunctionVar(Function* value)
        : _Base( value ), mDefault( true ) {}
    FunctionVar(const String& name, Function* value)
        : _Base( name, value ), mDefault( true ) {}
    virtual bool hasDefault() const { return this->mDefault; }

    virtual void pack(Bridge& bridge) const { bridge.pack( mValue ); }
    virtual void unpack(Bridge& bridge) { bridge.unpack( mValue ); }
    virtual int check(Bridge& bridge) const { return bridge.check( mValue ); }

    virtual ScriptVar* clone() const { return new _Self(mName, mValue); }
    virtual String toString() const;

    USS_TYPEID
};


// заглушка, не вызывается.
inline bool ScriptVar::is(const ScriptVar* to) const { assert(!"Stub called. This is very bad."); return false; }

inline bool ScriptVar::is(const BoolVar* to) const      { return to->is(this); }
inline bool ScriptVar::is(const IntVar* to) const       { return to->is(this); }
inline bool ScriptVar::is(const RealVar* to) const      { return to->is(this); }
inline bool ScriptVar::is(const CharVar* to) const      { return to->is(this); }
inline bool ScriptVar::is(const StringVar* to) const    { return to->is(this); }
inline bool ScriptVar::is(const IteratorVar* to) const  { return to->is(this); }
inline bool ScriptVar::is(const ScriptableVar* to) const{ return to->is(this); }
inline bool ScriptVar::is(const HashVar* to) const      { return to->is(this); }
inline bool ScriptVar::is(const FunctionVar* to) const  { return to->is(this); }

template<class OStream>
OStream& operator <<(OStream& os, const ScriptVar& var)
{
    return os << var.toString();
}

}// namespace Script

#endif // __ScriptVar_H__
