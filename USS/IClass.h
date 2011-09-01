#ifndef __USSIClass_H__
#define __USSIClass_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"
#include "USS/IObject.h"

namespace Script {

    class _ScriptExport IClass : public IObject
    {
    public:
        virtual ~IClass() {}
        virtual const String& getName() const = 0;

        /// @return Список всех полей объекта.
        virtual FieldList       getFields() const = 0;
        /// @return Список всех методов объекта, исключая конструкторы.
        virtual MethodList      getMethods() const = 0;
        /// @return Список всех свойств объекта.
        virtual PropertyList    getProperties() const = 0;
        /** @return Список вычислителей, которыми можно вычислить этот объект.
            Если этот список пустой, объект нельзя вычислять, как функцию.
        */
        virtual EvaluatorList getEvaluators() const = 0;
        /** @return Список индексаторов, которыми можно индексировать этот объект.
            Если этот список пустой, объект нельзя индексировать.
        */
        virtual IndexatorList getIndexators() const = 0;


        IProperty* getProperty(const String& name) const;
    };
}// namespace Script

#endif // __USSIClass_H__