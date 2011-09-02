
#include "OLS/OgreLogManager.h"
#include "USS/ScriptVar.h"
#include "USS/ScriptVarList.h"
#include "USS/IMethod.h"
#include "USS/IProperty.h"
#include "USS/IIndexator.h"

#include "USS/demo/Pair.h"
#include "USS/demo/PairObject.h"

namespace test {
    struct Deleter
    {
        template<typename T>
        void operator()(const T* p) const
        {
            delete p;
            p = NULL;
        }
    };
    // метод key()
    class Method_key : public IMethod
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Return key part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "key"; return s; }

    public:// IMethod
        virtual ScriptVarListPtr inArgs() const {
            return ScriptVarListPtr(new ScriptVarList());
        }
        virtual ScriptVarListPtr outArgs() const {
            ScriptVarListPtr args(new ScriptVarList());
            args->add(new StringVar());
            return args;
        }
        virtual void eval(IScriptable* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
            PairObject* o = static_cast<PairObject*>(object);
            static_cast<StringVar*>(outArgs[0])->value() = o->key();
        }
    };
    // метод value()
    class Method_value : public IMethod
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Return value part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "value"; return s; }

    public:// IMethod
        virtual ScriptVarListPtr inArgs() const {
            return ScriptVarListPtr(new ScriptVarList());
        }
        virtual ScriptVarListPtr outArgs() const {
            ScriptVarListPtr args(new ScriptVarList());
            args->add(new IntVar());
            return args;
        }
        virtual void eval(IScriptable* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
            PairObject* o = static_cast<PairObject*>(object);
            static_cast<IntVar*>(outArgs[0])->value() = o->value();
        }
    };

    
    // метод setKey()
    class Method_setKey : public IMethod
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Set key part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "setKey"; return s; }

    public:// IMethod
        virtual ScriptVarListPtr inArgs() const {
            ScriptVarListPtr args(new ScriptVarList());
            args->add(new StringVar());
            return args;
        }
        virtual ScriptVarListPtr outArgs() const {
            return ScriptVarListPtr(new ScriptVarList());
        }
        virtual void eval(IScriptable* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
            PairObject* o = static_cast<PairObject*>(object);
            o->setKey( static_cast<StringVar*>(inArgs[0])->value() );
        }
    };

    // метод setValue()
    class Method_setValue : public IMethod
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Set value part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "setValue"; return s; }

    public:// IMethod
        virtual ScriptVarListPtr inArgs() const {
            ScriptVarListPtr args(new ScriptVarList());
            args->add(new IntVar());
            return args;
        }
        virtual ScriptVarListPtr outArgs() const {
            return ScriptVarListPtr(new ScriptVarList());
        }
        virtual void eval(IScriptable* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const {
            PairObject* o = static_cast<PairObject*>(object);
            o->setValue( static_cast<IntVar*>(inArgs[0])->value() );
        }
    };
    // свойство first
    class Property_first : public IProperty
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Return key part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "first"; return s; }

    public:// IProperty
        virtual ScriptVarPtr propArg() const {
            return ScriptVarPtr(new StringVar());
        }
        virtual void get(const IScriptable* object, ScriptVarPtr& propArg) const {
            const PairObject* o = static_cast<const PairObject*>(object);
            static_cast<StringVar&>(*propArg).value() = o->key();
        }
        virtual void set(IScriptable* object, const ScriptVarPtr& propArg) {
            PairObject* o = static_cast<PairObject*>(object);
            o->setKey( static_cast<const StringVar&>(*propArg).value() );
        }
    };
    // свойство second
    class Property_second : public IProperty
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Return value part of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "second"; return s; }

    public:// IProperty
        virtual ScriptVarPtr propArg() const {
            return ScriptVarPtr(new IntVar());
        }
        virtual void get(const IScriptable* obj, ScriptVarPtr& propArg) const {
            const PairObject* o = static_cast<const PairObject*>(obj);
            static_cast<IntVar&>(*propArg).value() = o->value();
        }
        virtual void set(IScriptable* obj, const ScriptVarPtr& propArg) {
            PairObject* o = static_cast<PairObject*>(obj);
            o->setValue( static_cast<const IntVar&>(*propArg).value() );
        }
    };
    // свойство OnChange
    class Property_OnChange : public IProperty
    {
    public:// IDocumentable
        virtual const String& doc() const { static String s = "Callback on change key or value of pair."; return s; }

    public:// IField
        virtual const String& getName() const { static String s = "OnChange"; return s; }

    public:// IProperty
        virtual ScriptVarPtr propArg() const {
            return ScriptVarPtr(new FunctionVar());
        }
        virtual void get(const IScriptable* obj, ScriptVarPtr& propArg) const {}
        virtual void set(IScriptable* obj, const ScriptVarPtr& propArg) {
            PairObject* o = static_cast<PairObject*>(obj);
            o->addEventHandler( "OnChange", static_cast<const FunctionVar&>(*propArg).value() );
        }
    };

/*
class IntIterator : public java::Iterator<ScriptVarList> {
    typedef IntIterator _Self;
    int mValue;
public:
    IntIterator(int value) : mValue( value ) {}
    virtual bool hasNext() const { return mValue > 0; }
    virtual const_reference next() {
        //FIXME Утечка памяти!
        ScriptVarList* ret = new ScriptVarList();
        ret->push_back( new IntVar(mValue--) );
        return *ret; 
    }

    virtual java::Iterator<ScriptVarList>* clone() const { return new _Self(mValue); }
};
*/
    class IntIndexator : public IIndexator {
    public:
        virtual ScriptVarPtr keyArg() const {
            return ScriptVarPtr(new IntVar());
        }
        virtual ScriptVarPtr valueArg() const {
            return ScriptVarPtr(new StringVar());
            //return ScriptVarPtr(new IteratorVar());
        }
        virtual void get(const IScriptable* object, const ScriptVarPtr& keyArg, ScriptVarPtr& valueArg) const {
            const PairObject* o = static_cast<const PairObject*>(object);
            const IntVar& iVar = static_cast<const IntVar&>(*keyArg);
            // пример замены типа "на месте"
            if ( iVar.value() == 1 )
            {
                valueArg.reset( new StringVar(StringUtil::BLANK, o->key()) );
                return;
            }
            if ( iVar.value() == 2 )
            {
                valueArg.reset( new IntVar(o->value()) );
                return;
            }
            // Переменная, содержащая итератор по int
            IteratorVar& itVar = static_cast<IteratorVar&>(*valueArg);

            //itVar.value() = new IntIterator(iVar.value());
        }
        virtual void set(IScriptable* object, const ScriptVarPtr& keyArg, const ScriptVarPtr& valueArg) {
            
        }
    public:// IDocumentable
        virtual const String& doc() const
        {
            static String doc("If ${1} == 1, return key. If ${1} == 2, return value. Otherwise return iterator, traversal from ${1} to 1.");
            return doc;
        }
    };

    PairClass::PairClass()
        : mClassStatic(new PairClassStatic())
    {
        mMethods.insert(new Method_key());
        mMethods.insert(new Method_value());
        mMethods.insert(new Method_setKey());
        mMethods.insert(new Method_setValue());

        mProperties.insert(new Property_first());
        mProperties.insert(new Property_second());
        mProperties.insert(new Property_OnChange());

        mIndexators.push_back(new IntIndexator());
    }

    PairClass::~PairClass()
    {
        std::for_each( mMethods.begin(), mMethods.end(), Deleter() );
        std::for_each( mProperties.begin(), mProperties.end(), Deleter() );
        std::for_each( mIndexators.begin(), mIndexators.end(), Deleter() );

        mMethods.clear();
        mProperties.clear();
        mIndexators.clear();
    }
    const String& PairClass::doc() const
    {
        static String doc("Pair (string, int).");
        return doc;
    }

    String PairClass::toString() const
    {
        return "class Pair";
    }

    const String& PairClass::getName() const
    {
        static String name = "Pair";
        return name;
    }

    const IClass* PairClass::getClass() const
    {
        return mClassStatic;
    }

    FieldList PairClass::getFields() const
    {
        FieldList result;
        result.insert( mMethods.begin(), mMethods.end() );
        result.insert( mProperties.begin(), mProperties.end() );
        return result;
    }
    MethodList PairClass::getMethods() const
    {
        return mMethods;
    }
    PropertyList PairClass::getProperties() const
    {
        return mProperties;
    }
    EvaluatorList PairClass::getEvaluators() const
    {
        return EvaluatorList();
    }
    IndexatorList PairClass::getIndexators() const
    {
        return mIndexators;
    }
//========================================================================
// Конструкторы
//========================================================================
ScriptVarListPtr PairClass::C0::inArgs() const
{
    return ScriptVarListPtr(new ScriptVarList());
}
void PairClass::C0::eval(IScriptable* o, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
{
    static_cast<ScriptableVar*>(outArgs[0])->value().bind(new PairObject(
        static_cast<PairClass*>(o)
    ));
}
const String&  PairClass::C0::doc() const
{
    static String doc("Create Pair with default perameters - (0, \"\").");
    return doc;
}
//------------------------------------------------------------------------
ScriptVarListPtr PairClass::C1::inArgs() const
{
    ScriptVarListPtr args(new ScriptVarList());
    args->add( new StringVar("key") );
    return args;
}
void PairClass::C1::eval(IScriptable* o, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
{
    static_cast<ScriptableVar*>(outArgs[0])->value().bind(new PairObject(
        static_cast<StringVar*>(inArgs[0])
      , static_cast<PairClass*>(o)
    ));
}
const String&  PairClass::C1::doc() const
{
    static String doc("Create Pair with specified ${1} and ${2} == 0.");
    return doc;
}
//------------------------------------------------------------------------
ScriptVarListPtr PairClass::C2::inArgs() const
{
    ScriptVarListPtr args(new ScriptVarList());
    args->add( new StringVar("key") );
    args->add( new IntVar("value") );
    return args;
}
void PairClass::C2::eval(IScriptable* o, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
{
    static_cast<ScriptableVar*>(outArgs[0])->value().bind(new PairObject(
        static_cast<StringVar*>(inArgs[0])
      , static_cast<IntVar*>(inArgs[1])
      , static_cast<PairClass*>(o)
    ));
}
const String&  PairClass::C2::doc() const
{
    static String doc("Create Pair from ${1} and ${2}.");
    return doc;
}
//------------------------------------------------------------------------
ScriptVarListPtr PairClass::C3::inArgs() const
{
    ScriptVarListPtr args(new ScriptVarList());
    args->add( new StringVar("key") );
    args->add( new IntVar("value") );
    args->add( new FunctionVar("OnChange") );
    return args;
}
void PairClass::C3::eval(IScriptable* o, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
{
    static_cast<ScriptableVar*>(outArgs[0])->value().bind(new PairObject(
        static_cast<StringVar*>(inArgs[0])
      , static_cast<IntVar*>(inArgs[1])
      , static_cast<FunctionVar*>(inArgs[2])
      , static_cast<PairClass*>(o)
    ));
}
const String&  PairClass::C3::doc() const
{
    static String doc("Create Pair from ${1} and ${2} and notification function ${3}.");
    return doc;
}
//------------------------------------------------------------------------
    PairClassStatic::PairClassStatic() 
    {
        mEvaluators.push_back(new PairClass::C0());
        mEvaluators.push_back(new PairClass::C1());
        mEvaluators.push_back(new PairClass::C2());
        mEvaluators.push_back(new PairClass::C3());
    }

    PairClassStatic::~PairClassStatic()
    {
        std::for_each( mEvaluators.begin(), mEvaluators.end(), Deleter() );

        mEvaluators.clear();
    }

    const String& PairClassStatic::doc() const
    {
        return StringUtil::BLANK;
    }

    String PairClassStatic::toString() const
    {
        return StringUtil::BLANK;
    }

    const String& PairClassStatic::getName() const
    {
        static String s("PairClass");
        return s;
    }

    const IClass* PairClassStatic::getClass() const
    {
        return NULL;
    }

    FieldList PairClassStatic::getFields() const
    {
        return FieldList();
    }
    MethodList PairClassStatic::getMethods() const
    {
        return MethodList();
    }
    PropertyList PairClassStatic::getProperties() const
    {
        return PropertyList();
    }
    EvaluatorList PairClassStatic::getEvaluators() const
    {
        return mEvaluators;
    }
    IndexatorList PairClassStatic::getIndexators() const
    {
        return IndexatorList();
    }
//========================================================================
// Object
//========================================================================

String PairObject::toString() const 
{
    StringStream ss;
    ss << "instance " << mClass->getName() << " [key="
       << mObj->key() << ",value=" << mObj->value() << "]";
    return ss.str();
}

/*
-- В lua:
-----------------------------------------------
local obj = Pair:new()  -- "<default> created."
--local obj = Pair()    -- второй вариант
print(obj)              -- "userdata"
print(obj:getName())    -- "<default>"
obj:setName('obj')
print(obj:getName())    -- "obj"
obj:destroy()           -- "obj destroyed."
print(obj)              -- "nil"

## В Python:
###############################################
obj = Pair()            # "<default> created."
print(obj)              # 
print(obj.getName())    # "<default>"
obj.setName('obj')
print(obj.getName())    # "obj"
obj.destroy()           # "obj destroyed."
print(obj)              # "nil"
 */
}