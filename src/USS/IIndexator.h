#ifndef __USSIIndexator_H__
#define __USSIIndexator_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "USS/IDocumentable.h"

namespace Script {
    class IIndexator : public IDocumentable
    {
    public:
        virtual ~IIndexator() {}
        /// Возвращает ожидаемый входной параметр.
        virtual ScriptVarPtr keyArg()   const = 0;
        /** Возвращает тип возвращаемого параметра.
        @return
            placeholder, в который будет помещен результат.
        */
        virtual ScriptVarPtr valueArg() const = 0;

        /** Вычисляет значение индексации объекта @c context аргументом @c keyArg
            и записывает результат в @c valueArg. Скриптовый движок гарантирует,
            что тип входного параметра соответствуют @link keyArg()@endlink,
            а тип выходного параметра соответствуют @link valueArg()@endlink.
        @param object
            Индексируемый объект.
        @param keyArg
            Входной аргумент, содержащий данные из скриптового движка о индексе.
        @param valueArg
            Выходной аргумент, которые нужно заполнить вычисленным значением.

        @note
            Передача ScriptVarPtr вместо ScriptVar - вынужденная мера, так как в противном
            случае невозможно изменить тип на месте, потому что указатель, получаемый при
            разименовании auto_ptr не является указателем, который хранится в auto_ptr,
            а только указывает на то же самое место в памяти. Под отладчиком все работает,
            как и задумывалось, а без него - крашится.
        */
        virtual void get(const IScriptable* object, const ScriptVarPtr& keyArg, ScriptVarPtr& valueArg) const = 0;

        /** Записывает по индексу @c keyArg указанного объекта @c context @c valueArg.
            Скриптовый движок гарантирует, что типы параметров соответствуют 
            @link keyArg()@endlink и @link valueArg()@endlink соответственно.
        @param object
            Индексируемый объект.
        @param keyArg
            Входной аргумент, содержащий данные из скриптового движка о индексе.
        @param valueArg
            Входной аргумент, содержащий данные из скриптового движка о значении.

        @note
            Передача ScriptVarPtr вместо ScriptVar - вынужденная мера, так как в противном
            случае невозможно изменить тип на месте, потому что указатель, получаемый при
            разименовании auto_ptr не является указателем, который хранится в auto_ptr,
            а только указывает на то же самое место в памяти. Под отладчиком все работает,
            как и задумывалось, а без него - крашится.
        */
        virtual void set(IScriptable* object, const ScriptVarPtr& keyArg, const ScriptVarPtr& valueArg) = 0;
    };
}// namespace Script

#endif // __USSIIndexator_H__