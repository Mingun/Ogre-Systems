
#include "OLS/OgreLogManager.h"
#include "USS/demo/Pair.h"

namespace test {
    using namespace Ogre;
    Pair::Pair() 
        : mKey( "<default>" ), mValue( 0 )
    {
    }
    Pair::Pair(const String& key)
        : mKey( key ), mValue( 0 )
    {
    }
    Pair::Pair(const String& key, int value) 
        : mKey( key ), mValue( value )
    {
    }
    Pair::~Pair() 
    {
        LogManager::getSingleton().logMessage( mKey + " destroyed." );
    }
/*
-- В lua:
-----------------------------------------------
local obj = Pair:new()  -- "<default> created."
--local obj = Pair()    -- второй вариант
print(obj)              -- "userdata"
print(obj:getName())    -- "<default>"
obj:setName('obj')
print(obj:getName())    -- "obj"
obj:destroy()           -- "obj destroyed."
print(obj)              -- "nil"

## В Python:
###############################################
obj = Pair()            # "<default> created."
print(obj)              # 
print(obj.getName())    # "<default>"
obj.setName('obj')
print(obj.getName())    # "obj"
obj.destroy()           # "obj destroyed."
print(obj)              # "nil"
 */
}