
#include "OLS/OgreLogManager.h"
#include "Pair.h"

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
        StringStream ss;
        ss << "~Pair():[key=" << mKey << ",value=" << mValue << ']';
        LogManager::getSingleton().logMessage( ss.str() );
    }
/*
-- В lua:
-----------------------------------------------
local obj = Pair()
print(obj)              -- "inctance Pair [key=<default>,value=0]"
print(obj:getName())    -- "<default>"
obj:setName('obj')
print(obj:getName())    -- "obj"
obj:destroy()           -- "~Pair()"

## В Python:
###############################################
obj = Pair()            # "<default> created."
print(obj)              # 
print(obj.getName())    # "<default>"
obj.setName('obj')
print(obj.getName())    # "obj"
obj.destroy()           # "~Pair()"
 */
}