/** @file Utility.h
Предоставляет часто используемые классы для создания полей
объектов.
<p>Пример создания конструктора объекта.
@code
#define ADD_FIELD(Field) f = new Field(this); mFields.insert( FieldList::value_type(f->getName(), f) );
class Object_Pair : public util::Object
{
    typedef Ogre::vector<const IIndexator*>::type IndexatorList;

    class Method_key : public util::SimpleMethod
    {
        typedef util::SimpleMethod _Base;
    private:
        Pair* mObj;
    public:
        Method_key(Pair* obj) : _Base(), mObj( obj ) {}
        Pair* object() const { return mObj; }
    public:// IObject
        virtual const String& getName() const { static String name("key"); return name; }

        /// Возвращает строковое представление объекта.
        virtual String toString() const {
            return "Method: string key()";
        }
    public:// IEvaluator
        virtual ScriptVarListPtr inArgs() const
        { return ScriptVarListPtr(new ScriptVarList()); }
        virtual ScriptVarListPtr outArgs() const
        {
            ScriptVarListPtr args(new ScriptVarList());
            args->add( new StringVar() );
            return args;
        }
        virtual void eval(ICallable* context, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
        {
            static_cast<StringVar*>(outArgs[0])->value() = static_cast<Method_key*>(context)->object()->key();
        }
    public:// IDocumentable
        virtual const String& doc() const
        { static String doc("Return key/first part of pair."); return doc; }
    };

    class Method_value : public util::SimpleMethod
    {
        typedef util::SimpleMethod _Base;
    private:
        Pair* mObj;
    public:
        Method_key(Pair* obj) : _Base(), mObj( obj ) {}
        Pair* object() const { return mObj; }
    public:// IObject
        virtual const String& getName() const { static String name("value"); return name; }

        /// Возвращает строковое представление объекта.
        virtual String toString() const {
            return "Method: int value()";
        }
    public:// IEvaluator
        virtual ScriptVarListPtr inArgs() const
        { return ScriptVarListPtr(new ScriptVarList()); }
        virtual ScriptVarListPtr outArgs() const
        {
            ScriptVarListPtr args(new ScriptVarList());
            args->add( new IntVar() );
            return args;
        }
        virtual void eval(ICallable* context, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
        {
            static_cast<IntVar*>(outArgs[0])->value() = static_cast<Method_value*>(context)->object()->value();
        }
    public:// IDocumentable
        virtual const String& doc() const
        { static String doc("Return value/second part of pair."); return doc; }
    };

    class Deleter {
    public:
        template<typename T>
        void operator()(const T* p) const {
            delete p;
            p = NULL;
        }
    };

private:
    IndexatorList mIndexators;
public:
    Object_Pair() 
    {
        IObject* f;
        ADD_FIELD(Method_key);
        ADD_FIELD(Method_value);
        // ADD_FIELD(Property_first);
        // ADD_FIELD(Property_second);
        // ADD_FIELD(Constructor);
        
        // mIndexators.push_back(new IntIndexator());
    }

    virtual ~Object_Pair() 
    {
        std::for_each( mIndexators.begin(), mIndexators.end(), Deleter() );
        mIndexators.clear();
    }

public:// IObject
    virtual const String& getName() const { static String name("Pair"); return name; }

    virtual String toString() const { 
        return "Pair:0x" + StringUtil::toHexString(static_cast<void*>(this));
    }

public:// IIndexable
    virtual IndexatorIteratorPtr indexators() const
    { return IndexatorIteratorPtr(java::utils::make_stl_container_iterator(mIndexators)); }

public:// IDocumentable
    virtual const String& doc() const { static String doc("Pair of (string,int)."); return doc; }
};
@endcode
*/

#ifndef __Utility_H__
#define __Utility_H__

#pragma once

#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "USS/Object.h"
#include "Common/JavaIteratorContainerAdaptors.h"
#include "Common/JavaIteratorCombinatorAdaptors.h"
#include "OMS/OgreSharedPtr.h"

namespace Script { namespace utils {

    class Method : public IField   // Это поле...
                 , public ICallable// ...являющееся функцией
    {
        typedef Ogre::vector<const IEvaluator*>::type EvaluatorList;
        class Deleter {
        public:
            void operator()(EvaluatorList::value_type p) const {
                delete p;
                p = NULL;
            }
        };

    protected:
        EvaluatorList mEvaluators;
        String mName;
        String mDoc;
    public:
        Method(const String& name, const String& doc)
            : mName( name ), mDoc( doc ) {}
        virtual ~Method() 
        {
            std::for_each( mEvaluators.begin(), mEvaluators.end(), Deleter() );
            mEvaluators.clear();
        }
    public:// IField
        const String& getName() const { return mName; }

    public:// IScriptable
        virtual ICallable*  asCallable()  { return this; }
        virtual IIndexable* asIndexable() { return NULL; }
        virtual IContainer* asContainer() { return NULL; }
        virtual IProperty*  asProperty()  { return NULL; }
        virtual IObject*    asObject()    { return NULL; }
        virtual IFactory*   asFactory()   { return NULL; }
        virtual const ICallable*  asCallable()  const { return this; }
        virtual const IIndexable* asIndexable() const { return NULL; }
        virtual const IContainer* asContainer() const { return NULL; }
        virtual const IProperty*  asProperty()  const { return NULL; }
        virtual const IObject*    asObject()    const { return NULL; }
        virtual const IFactory*   asFactory()   const { return NULL; }

        String toString() const { return "method " + mName; }

    public:// ICallable
        virtual EvaluatorIteratorPtr callers() const
        { return EvaluatorIteratorPtr(java::utils::make_stl_container_iterator(mEvaluators)); }

    public:// IDocumentable
        const String& doc() const { return mDoc; }
    };

    class SimpleMethod : public IField    // Это поле...
                       , public ICallable // ...являющееся функцией...
                       , public IEvaluator// ...только с одной сигнатурой
    {
    protected:
        String mName;
        String mDoc;
    public:
        SimpleMethod(const String& name, const String& doc)
            : mName( name ), mDoc( doc ) {}

    public:// IField
        const String& getName() const { return mName; }

    public:// IScriptable
        virtual ICallable*  asCallable()  { return this; }
        virtual IIndexable* asIndexable() { return NULL; }
        virtual IContainer* asContainer() { return NULL; }
        virtual IProperty*  asProperty()  { return NULL; }
        virtual IObject*    asObject()    { return NULL; }
        virtual IFactory*   asFactory()   { return NULL; }
        virtual const ICallable*  asCallable()  const { return this; }
        virtual const IIndexable* asIndexable() const { return NULL; }
        virtual const IContainer* asContainer() const { return NULL; }
        virtual const IProperty*  asProperty()  const { return NULL; }
        virtual const IObject*    asObject()    const { return NULL; }
        virtual const IFactory*   asFactory()   const { return NULL; }

        String toString() const { return "method " + mName; }

    public:// ICallable
        virtual EvaluatorIteratorPtr callers() const
        { return EvaluatorIteratorPtr(new java::OneIterator<const IEvaluator*>(this)); }

    public:// IDocumentable
        const String& doc() const { return mDoc; }
    };

    class Property : public IProperty// Это поле, являющееся свойством
    {
    protected:
        String mName;
        String mDoc;
    public:
        Property(const String& name, const String& doc)
            : mName( name ), mDoc( doc ) {}
    public:// IField
        const String& getName() const { return mName; }

    public:// IScriptable
        virtual ICallable*  asCallable()  { return NULL; }
        virtual IIndexable* asIndexable() { return NULL; }
        virtual IContainer* asContainer() { return NULL; }
        virtual IProperty*  asProperty()  { return this; }
        virtual IObject*    asObject()    { return NULL; }
        virtual IFactory*   asFactory()   { return NULL; }
        virtual const ICallable*  asCallable()  const { return NULL; }
        virtual const IIndexable* asIndexable() const { return NULL; }
        virtual const IContainer* asContainer() const { return NULL; }
        virtual const IProperty*  asProperty()  const { return this; }
        virtual const IObject*    asObject()    const { return NULL; }
        virtual const IFactory*   asFactory()   const { return NULL; }

        String toString() const { return "property " + mName; }

    public:// IDocumentable
        const String& doc() const { return mDoc; }
    };

    class ROProperty : public Property
    {
    public:
        ROProperty(const String& name, const String& doc)
            : Property( name, doc ) {}

    public:// IScriptable
        String toString() const { return "R/O property " + mName; }

    public:// IProperty
        virtual void set(IContainer*, const ScriptVar&) {}
    };

    class WOProperty : public Property
    {
    public:
        WOProperty(const String& name, const String& doc)
            : Property( name, doc ) {}

    public:// IScriptable
        String toString() const { return "W/O property " + mName; }

    public:// IProperty
        virtual void get(const IContainer*, ScriptVar&) const {}
    };

    class BaseFactory : public IFactory
    {
    protected:
        typedef Ogre::map<String, const IField*>::type FieldList;
        typedef Ogre::vector<const IEvaluator*>::type  ConstructorList;

        class FieldDeleter {
        public:
            void operator()(FieldList::value_type& p) const {
                delete p.second;
                p.second = NULL;
            }
        };
        class ConstructorDeleter {
        public:
            void operator()(ConstructorList::value_type p) const {
                delete p;
                p = NULL;
            }
        };

    protected:
        /// Тип фабрики. Под этим именем объект будет доступен в скриптах.
        String          mFactoryName;
        /// Список конструкторов объекта.
        ConstructorList mConstructors;
        /// Список полей объекта.
        FieldList       mFields;
    public:
        BaseFactory(const Ogre::String& factoryName)
            : mFactoryName( factoryName ) {}
        virtual ~BaseFactory() 
        {
            std::for_each( mFields.begin(), mFields.end(), FieldDeleter() );
            mFields.clear();
            std::for_each( mConstructors.begin(), mConstructors.end(), ConstructorDeleter() );
            mConstructors.clear();
        }
        virtual const String& getType() const { return mFactoryName; }

    public:// IScriptable
        virtual String toString() const { return "class " + mFactoryName; }

    public:// ICallable
        virtual EvaluatorIteratorPtr callers() const
        { return EvaluatorIteratorPtr(java::utils::make_stl_container_iterator(mConstructors)); }

    public:// IContainer
        virtual IScriptable* getField(const String& fieldName) const
        {
            FieldList::const_iterator it = mFields.find( fieldName );
            if ( it != mFields.end() )
                return const_cast<IField*>(it->second);
            return NULL;
        }
        virtual ScriptableIteratorPtr fields() const
        {
            return ScriptableIteratorPtr(java::utils::make_stl_container_iterator(mFields));
        }
    };

    template<typename T, class F>
    class BaseObject : public IObject   // Это объект...
                     , public IContainer// ...содержащий другие объекты - поля
    {
    public:
        typedef SharedPtr<T>    ObjectPtr;
        //typedef SharedPtr<F>    FactoryPtr;
        // в противном случае последний удаленный объект удалит фабрику
        // (если только не хранить фабрику в умном указателе)
        typedef F*              FactoryPtr;
    protected:
        ObjectPtr  mObj;
        FactoryPtr mFactory;

    protected:
        BaseObject(ObjectPtr obj, FactoryPtr factory)
            : mObj( obj )
            , mFactory( factory ) {}

    public:// IScriptable
        virtual ICallable*  asCallable()  { return NULL; }
        virtual IIndexable* asIndexable() { return NULL; }
        virtual IContainer* asContainer() { return this; }
        virtual IProperty*  asProperty()  { return NULL; }
        virtual IObject*    asObject()    { return this; }
        virtual IFactory*   asFactory()   { return NULL; }
        virtual const ICallable*  asCallable()  const { return NULL; }
        virtual const IIndexable* asIndexable() const { return NULL; }
        virtual const IContainer* asContainer() const { return this; }
        virtual const IProperty*  asProperty()  const { return NULL; }
        virtual const IObject*    asObject()    const { return this; }
        virtual const IFactory*   asFactory()   const { return NULL; }

    public:// IObject
        virtual IFactory& getFactory() const { assert( mFactory ); return *mFactory; }
        virtual const String& doc() const { assert( mFactory ); return mFactory->doc(); }

    public:// IContainer
        virtual ScriptableIteratorPtr fields() const
        { return mFactory->fields(); }
        virtual IScriptable* getField(const String& fieldName) const
        { return mFactory->getField( fieldName ); }
    };

}} // namespace Script::utils
#endif // __Utility_H__