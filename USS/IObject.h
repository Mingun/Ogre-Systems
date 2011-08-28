#ifndef __USSIObject_H__
#define __USSIObject_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreString.h"
#include "USS/IScriptable.h"

namespace Script {
    class IObject : public IScriptable
    {
    public:
        virtual ~IObject() {}
        /// @return Класс объекта, которым является этот объект.
        virtual const IClass* getClass() const = 0;
    };
}// namespace Script

#endif // __USSIObject_H__