#ifndef __USSIField_H__
#define __USSIField_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreString.h"
#include "USS/IDocumentable.h"

namespace Script {
    class IField : public IDocumentable
    {
    public:
        enum TYPE 
        {
            Method, Property, Constructor
        };
    public:
        virtual ~IField() {}
        /// @return Имя поля.
        virtual const String& getName() const = 0;

        virtual TYPE getType() const = 0;
    };
    
    inline bool IFieldPredicate::operator()(const IField* _Left, const IField* _Right) const
	{
        return _Left->getName() < _Right->getName();
	}
}// namespace Script

#endif // __USSIField_H__