#ifndef __USSIScriptable_H__
#define __USSIScriptable_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "USS/IDocumentable.h"

namespace Script {
    class IScriptable : public IDocumentable
    {
    public:
        virtual ~IScriptable() {}
        /// @return Строковое представление объекта.
        virtual String toString() const = 0;
    };
}// namespace Script

#endif // __USSIScriptable_H__