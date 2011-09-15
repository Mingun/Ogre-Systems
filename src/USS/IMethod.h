#ifndef __USSIMethod_H__
#define __USSIMethod_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "USS/IField.h"
#include "USS/IEvaluator.h"

namespace Script {
    class IMethod : public IField, public IEvaluator
    {
    public:
        virtual ~IMethod() {}
        virtual TYPE getType() const { return Method; }
    };
}// namespace Script

#endif // __USSIMethod_H__