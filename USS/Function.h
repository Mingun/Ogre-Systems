#ifndef __USSFunction_H__
#define __USSFunction_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
//#include "USS/ScriptManager.h"

namespace Script {

    class _ScriptExport Function : public ScriptableAlloc
    {
    protected:
        /// Имя файла, в котором определена функция.
        String mFileName;
        /// Строчка в файле, где начинается определение функции.
        int    mLine;
    public:
        Function(const String& fileName, int line = -1)
            : mFileName( fileName ), mLine( line ) {}
        virtual ~Function() {}
        inline int line() const { return mLine; }
        inline const String& fileName() const { return mFileName; }
        virtual void eval(const ScriptVarList& inArgs, ScriptVarList& outArgs) const = 0;
    };

    class _ScriptExport FileFunction : public Function
    {
        typedef Function _Base;
    private:
        /// Имя вызываемой функции.
        String mFunctionName;
    public:
        FileFunction(const String& fileName, const String& functionName = StringUtil::BLANK)
            : _Base( fileName ), mFunctionName( functionName ) {}
        virtual void eval(const ScriptVarList& inArgs, ScriptVarList& outArgs) const
        {
            //outArgs = ScriptManager::getSingleton().exec( mFileName, mFunctionName, inArgs );
        }
    };

}// namespace Script

#endif // __USSFunction_H__
