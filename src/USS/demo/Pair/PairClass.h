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
		public:\
			virtual ScriptVarListPtr inArgs() const;\
			virtual void eval(IScriptable* o, const ScriptVarList& inArgs, ScriptVarList& outArgs) const;\
			virtual const String& doc() const;\
		}

    class PairClass : public IClass
    {
        MethodList      mMethods;
        PropertyList    mProperties;
        IndexatorList   mIndexators;
        IClass*         mClassStatic;

        C(0); C(1); C(2); C(3);
        friend class PairClassStatic;
    public:
        PairClass();
        virtual ~PairClass();

    public:// IDocumentable
        virtual const String& doc() const;
    public:// IScriptable
        virtual String toString() const;
        virtual const IClass* getClass() const;
    public:// IClass
        virtual const String& getName() const;

        virtual FieldList       getFields() const;
        virtual MethodList      getMethods() const;
        virtual PropertyList    getProperties() const;
        virtual EvaluatorList   getEvaluators() const;
        virtual IndexatorList   getIndexators() const;
    };

    class PairClassStatic : public IClass
    {
        EvaluatorList      mEvaluators;
    public:
        PairClassStatic();
        virtual ~PairClassStatic();
    public:// IDocumentable
        virtual const String& doc() const;
    public:// IScriptable
        virtual String toString() const;
        virtual const IClass* getClass() const;
    public:// IClass
        virtual const String& getName() const;

        virtual FieldList       getFields() const;
        virtual MethodList      getMethods() const;
        virtual PropertyList    getProperties() const;
        virtual EvaluatorList   getEvaluators() const;
        virtual IndexatorList   getIndexators() const;
    };
} // namespace test

#endif // __PairClass_H__