#ifndef __USSIConstructor_H__
#define __USSIConstructor_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "USS/IMethod.h"
#include "USS/ScriptVar.h"

namespace Script {
    class IConstructor : public IMethod
    {
    public:
        virtual ~IConstructor() {}
        virtual TYPE getType() const { return Constructor; }
        virtual const String& getName() const { return StringUtil::BLANK; }

        virtual ScriptVarListPtr outArgs() const {
            ScriptVarList *list = new ScriptVarList();
            list->add(new ScriptableVar());
            return ScriptVarListPtr(list); 
        }
    };
}// namespace Script

#endif // __USSIConstructor_H__