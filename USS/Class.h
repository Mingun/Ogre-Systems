#ifndef __USSClass_H__
#define __USSClass_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"


#include "Common/OgreString.h"
#include "USS/IClass.h"
#include "USS/IObject.h"

namespace Script {

    class _ScriptExport Class : public IClass, public IObject
    {
    public:
        virtual ~Class() {}
    };

}// namespace Script

#endif// __USSClass_H__