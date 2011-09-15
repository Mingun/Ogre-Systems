#ifndef __Object_H__
#define __Object_H__

#pragma once

#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"
#include "Common/OgreString.h"

namespace test {
    using namespace Ogre;

    //====================================================================
    // Задача: сделать этот класс доступным из скриптов, написанных на разных языках
    /// @ussexport
    class Pair
    {
        /// @uss{roproperty,key,key()}
        String  mKey;
        /// @uss{property,value,value(),setValue($1)}
        int     mValue;
    public:
/// @ussbegin
        /// Конструирует пару с ключом "<default>" и значением 0.
        Pair();
        /** Конструирует пару с указанным ключом и значением 0.
        @param key
            Ключ для пары.
        */
        Pair(const String& key);
        Pair(const String& key, int value);
        virtual ~Pair();
        void dummy();
/// @ussend
        void setKey(const String& key)  { mKey = key;       }
        void setValue(int value)        { mValue = value;   }
        /// Получает ключ.
        /// @return Значение ключа
        const String& key() const       { return mKey;      }
        /// Получает значение для ключа данной пары.
        int           value() const     { return mValue;    }
    };

/*
-- В lua:
-----------------------------------------------
local obj = Object:new()-- "<default> created."
--local obj = Object()  -- второй вариант
print(obj)              -- "userdata"
print(obj:getName())    -- "<default>"
obj:setName('obj')
print(obj:getName())    -- "obj"
obj:destroy()           -- "obj destroyed."
print(obj)              -- "nil"

## В Python:
###############################################
obj = Object()          # "<default> created."
print(obj)              # 
print(obj.getName())    # "<default>"
obj.setName('obj')
print(obj.getName())    # "obj"
obj.destroy()           # "obj destroyed."
print(obj)              # "none"
 */
}
#endif // __Object_H__