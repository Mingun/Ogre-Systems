#ifndef __PairObject_H__
#define __PairObject_H__

#pragma once

#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreString.h"

#include "USS/IScriptable.h"
#include "USS/Eventable.h"

#include "PairClass.h"
#include "Pair.h"

namespace test {
	using namespace Ogre;
	using namespace Script;

    template<class T, class C>
    class ObjectWrapper : public IScriptable   // Это объект
    {
    public:
        typedef SharedPtr<T>    ObjectPtr;
        //typedef SharedPtr<C>    ClassPtr;
        // в противном случае последний удаленный объект удалит фабрику
        // (если только не хранить фабрику в умном указателе)
        typedef const C*        ClassPtr;
    protected:
        ObjectPtr mObj;
        ClassPtr  mClass;

    protected:
        ObjectWrapper(ObjectPtr obj, ClassPtr cls)
            : mObj( obj )
            , mClass( cls ) {}

    public:// IScriptable
        virtual const IClass* getClass() const { assert( mClass ); return mClass; }
        virtual const String& doc() const { assert( mClass ); return mClass->doc(); }
    };

	class PairObject : public ObjectWrapper<Pair, PairClass>, public Eventable
	{
        typedef ObjectWrapper<Pair, PairClass> _Base;
	public:
		PairObject(ObjectPtr obj, ClassPtr cls)
			: _Base(obj, cls) {}
		PairObject(ClassPtr cls)
			: _Base(ObjectPtr(new Pair()), cls) {}
		PairObject(const StringVar* key, ClassPtr cls)
			: _Base(ObjectPtr(new Pair(key->value())), cls) {}
		PairObject(const StringVar* key, const IntVar* value, ClassPtr cls)
			: _Base(ObjectPtr(new Pair(key->value(), value->value())), cls) {}
		PairObject(const StringVar* key, const IntVar* value, const FunctionVar* onChangeEvent, ClassPtr cls)
			: _Base(ObjectPtr(new Pair(key->value(), value->value())), cls) 
		{
			addEventHandler( "OnChange", onChangeEvent->value() );
		}

        ~PairObject()
        {
            LOG("~PairObject()");
        }

		virtual String toString() const;

	public:
		// дублируем скриптабельный интерфейс контролируемого объекта, 
		// для удобства и применения хуков.
		inline void setKey(const String& key) 
		{
			ScriptVarList inArgs;
			inArgs.add( new StringVar("old", mObj->key()) );
			inArgs.add( new StringVar("new", key) );
			fireEvent( "OnChange", inArgs );
			mObj->setKey( key );
		}
		inline void setValue(int value)       { fireEvent("OnChange"); mObj->setValue( value ); }
		inline const String& key() const      { return mObj->key();      }
		inline int  value() const             { return mObj->value();    }
	};
} // namespace test
#endif // __PairObject_H__