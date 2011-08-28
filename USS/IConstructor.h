#ifndef __USSIConstructor_H__
#define __USSIConstructor_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "USS/IField.h"

namespace Script {
    class IConstructor : public IField
    {
    public:
        virtual ~IConstructor() {}
        virtual TYPE getType() const { return Constructor; }
        virtual const String& getName() const { return StringUtil::BLANK; }

        virtual ScriptVarListPtr inArgs()  const = 0;
        /** Создает объект.
        @param inArgs 
            Параметры консруктора с актуальными значениями.
        @return 
            Новый объект.
        */
        virtual IObject* create(const ScriptVarList& inArgs) const = 0;
    };
}// namespace Script

#endif // __USSIConstructor_H__