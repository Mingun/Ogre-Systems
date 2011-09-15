#ifndef __Eventable_H__
#define __Eventable_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreString.h"
#include "USS/ScriptVarList.h"

namespace Script {

/** Класс для поддержки скриптовых событий.
*/
class _ScriptExport Eventable : public ScriptableAlloc
{
    typedef Eventable _Self;
protected:
    /// Отображение eventName => eventHandler
    typedef HashMap<String, Function*> EventList;
protected:
    EventList mEvents;
public:
    virtual ~Eventable();

    /** Синхронно выполняет обработчик, если он есть, для события с указанным
        именем. Иными словами, управление вернется после выполнения обработчика.
    @param eventName
        Имя события, о котором надо сигнализировать обработчику.
    @param inArgs
        Аргументы, передаваемые обработчику события.
    @param outArgs
        Результаты, ожидаемые от обработчика события.
    */
    void fireEvent(const String& eventName, const ScriptVarList& inArgs = ScriptVarList::BLANK, ScriptVarList& outArgs = ScriptVarList::BLANK) const;

    /** Регистрирует указанную функцию как обработчик указанного события.
    @param eventName
        Событие, для которого регистрируется обработчик. Когда событие 
        будет возбуждено вызовом @link fireEvent@endlink, обработчик 
        будет выполнен.
    @param handler
        Обработчик. Если равен @c NULL, существующий обработчик удалиться.
    */
    inline void addEventHandler(const String& eventName, Function* handler)
    {
        removeEventHandler( eventName );
        if ( handler )
            mEvents.insert( EventList::value_type(eventName, handler) );
    }

    /** Удаляет обработчик указанного события.
    */
    void removeEventHandler(const String& eventName);
};

}// namespace Script;

#endif // __Eventable_H__