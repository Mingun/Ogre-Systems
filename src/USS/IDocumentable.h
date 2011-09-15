#ifndef __USSIDocumentable_H__
#define __USSIDocumentable_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"

namespace Script {
    /** Интерфейс для предоставления документации по скриптабельному объекту. 
    @remarks
        Важно, чтобы приложение не только давало доступ к своему API скриптам,
        но и чтобы это API было хорошо документировано. Введение обязательного
        интерфейса позволит хотя бы напомнить разработчику о том, что его 
        системой еще будут и пользоваться и надо бы предоставить описание для
        нее.
    */
    class IDocumentable
    {
    public:
        virtual ~IDocumentable() {}
        /// Возвращает документацию по объекту.
        virtual const String& doc() const = 0;
    };
}// namespace Script

#endif // __USSIDocumentable_H__