#ifndef __USSIProperty_H__
#define __USSIProperty_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "USS/IField.h"

namespace Script {
    class IProperty : public IField
    {
    public:
        virtual ~IProperty() {}
        virtual TYPE getType() const { return Property; }

        virtual ScriptVarPtr propArg() const = 0;
        
        /**
        @note
            Передача ScriptVarPtr вместо ScriptVar - вынужденная мера, так как в противном
            случае невозможно изменить тип на месте, потому что указатель, получаемый при
            разименовании auto_ptr не является указателем, который хранится в auto_ptr,
            а только указывает на то же самое место в памяти. Под отладчиком все работает,
            как и задумывалось, а без него - крашится.
        */
        virtual void get(const IScriptable* object, ScriptVarPtr& propArg) const = 0;

        /**
        @note
            Передача ScriptVarPtr вместо ScriptVar - вынужденная мера, так как в противном
            случае невозможно изменить тип на месте, потому что указатель, получаемый при
            разименовании auto_ptr не является указателем, который хранится в auto_ptr,
            а только указывает на то же самое место в памяти. Под отладчиком все работает,
            как и задумывалось, а без него - крашится.
        */
        virtual void set(IScriptable* object, const ScriptVarPtr& propArg) = 0;
    };
}// namespace Script

#endif // __USSIProperty_H__