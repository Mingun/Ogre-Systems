#ifndef __PairClass_H__
#define __PairClass_H__

#pragma once
#include "USS/IClass.h"
#include "USS/IConstructor.h"

namespace test {
    using namespace Script;
    
	#define C(n) \
	    class C##n : public IConstructor\
		{\
            const PairClass* mClass;\
		public:\
            C##n(const PairClass* cls) : mClass(cls) {}\
			virtual ScriptVarListPtr inArgs() const;\
			virtual IObject* create(const ScriptVarList& inArgs) const;\
			virtual const String& doc() const;\
		}

    class PairClass : public IClass
    {
        MethodList      mMethods;
        PropertyList    mProperties;
        ConstructorList mConstructors;
        IndexatorList   mIndexators;

        C(0); C(1); C(2); C(3);
    public:
        PairClass();
        ~PairClass();

    public:
        virtual const String& doc() const;
        virtual String toString() const;
        virtual const String& getName() const;

        virtual FieldList       getFields() const;
        virtual MethodList      getMethods() const;
        virtual PropertyList    getProperties() const;
        virtual ConstructorList getConstructors() const;
        virtual EvaluatorList   getEvaluators() const;
        virtual IndexatorList   getIndexators() const;
    };
} // namespace test

#endif // __PairClass_H__