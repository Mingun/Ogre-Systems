#ifndef __USSIEvaluator_H__
#define __USSIEvaluator_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "USS/IDocumentable.h"

namespace Script {
    class IEvaluator : public IDocumentable
    {
    public:
        virtual ~IEvaluator() {}
        /** Возвращает список типов принимаемых параметров. 
            Для функции это ее список аргументов. Для свойства это список
            допустимых типов, которые можно присвоить свойству.
        @return 
            Список ожидаемых входных параметров.
        */
        virtual ScriptVarListPtr inArgs()  const = 0;

        /** Возвращает список типов возвращаемых параметров.
        @return
            placeholder, в который будет помещен результат.
        */
        virtual ScriptVarListPtr outArgs() const = 0;

        /** Вычисляет значение поля, основываясь на аргументах @c inArgs
            и записывает результат в @c outArgs. Скриптовый движок гарантирует,
            что типы входных параметров соответствуют @link inArgs()@endlink,
            а типы выходных параметров соответствуют @link outArgs()@endlink.
        @param object
            Вычисляемый объект.
        @param inArgs
            Входные аргументы, содержащие данные из скриптового движка.
        @param outArgs
            Выходные аргументы, которые нужно заполнить вычисленными значениями.
        */
        virtual void eval(IScriptable* object, const ScriptVarList& inArgs, ScriptVarList& outArgs) const = 0;
    };
}// namespace Script

#endif // __USSIEvaluator_H__