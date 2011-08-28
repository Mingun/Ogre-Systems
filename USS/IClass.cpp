#include "USS/IClass.h"
#include "USS/IProperty.h"

namespace Script {
    IProperty* IClass::getProperty(const String& name) const
    {
        PropertyList list = getProperties();
        for ( PropertyList::const_iterator it = list.begin(); it != list.end(); ++it )
        {
            if ( (*it)->getName() == name )
                return *it;
        }
        return NULL;
    }
}// namespace Script