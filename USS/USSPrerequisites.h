#ifndef __USSPrerequisites_H_
#define __USSPrerequisites_H_

#include "Common/OgrePlatform.h"

//----------------------------------------------------------------------------
// Windows Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#   if defined( OGRE_STATIC_LIB )
        // Linux compilers don't have symbol import/export directives.
#       define _ScriptExport
#   else
#       if defined( USS_SERVER )
#           define _ScriptExport __declspec( dllexport )
#       else
#           if defined( __MINGW32__ )
#               define _ScriptExport
#           else
#               define _ScriptExport __declspec( dllimport )
#           endif
#       endif
#   endif

#endif // OGRE_PLATFORM == OGRE_PLATFORM_WIN32

//----------------------------------------------------------------------------
// Symbian Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_SYMBIAN
#   define _ScriptExport
#endif
//----------------------------------------------------------------------------
// Linux/Apple/Symbian Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_IPHONE || OGRE_PLATFORM == OGRE_PLATFORM_SYMBIAN

// Enable GCC symbol visibility
#   if defined( OGRE_GCC_VISIBILITY )
#       define _ScriptExport  __attribute__ ((visibility("default")))
#   else
#       define _ScriptExport
#   endif

#endif

//----------------------------------------------------------------------------

//#include "Common/OgreStdHeaders.h"
#include "OMS/OgreMemoryAllocatorConfig.h"
#include "OMS/OgreSharedPtr.h"

namespace java {
    template< typename T > class Iterator;
}

namespace Script {

    using namespace Ogre;
    typedef GeneralAllocatedObject ScriptableAlloc;
    
    class IDocumentable;
    class IScriptable;
    class Class;
    class IClass;
    class IField;
    class IMethod;
    class IProperty;
    class IConstructor;
    class IEvaluator;
    class IIndexator;
    class IModule;

    class ScriptManager;
    class ScriptEngine;
    class Bridge;
    class Hash;
    class Function;

    class ScriptVar;
    class BoolVar;
    class RealVar;
    class IntVar;
    class CharVar;
    class StringVar;
    template<typename E> class BaseIteratorVar;
    class IteratorVar;
    class ScriptableVar;
    class HashVar;
    class FunctionVar;

    class ScriptVarList;

    typedef java::Iterator<ScriptVarList>       Iterator;
    
    typedef std::auto_ptr<ScriptVar>            ScriptVarPtr;
    typedef std::auto_ptr<ScriptVarList>        ScriptVarListPtr;

    typedef SharedPtr<Iterator>                 IteratorPtr;


    struct IFieldPredicate : public std::binary_function<IField*, IField*, bool>
    {
        bool operator()(const IField* _Left, const IField* _Right) const;
    };

    typedef multiset<IField*, IFieldPredicate>::type  FieldList;
    typedef multiset<IMethod*, IFieldPredicate>::type MethodList;
    typedef set<IProperty*, IFieldPredicate>::type    PropertyList;
    typedef vector<IConstructor*>::type               ConstructorList;
    typedef vector<IEvaluator*>::type                 EvaluatorList;
    typedef vector<IIndexator*>::type                 IndexatorList;

    typedef SharedPtr<ScriptEngine> ScriptEnginePtr;
    typedef SharedPtr<IScriptable>  IScriptablePtr;
    typedef SharedPtr<IClass>       IClassPtr;
    typedef SharedPtr<IModule>      IModulePtr;

} // namespace Script

#endif // __USSPrerequisites_H_