#include "USS/Eventable.h"
#include "USS/Function.h"

namespace Script {

    class Deleter
    {
    public:
        template<typename T>
        void operator()(T& p) const
        {
            delete p.second;
            p.second = NULL;
        }
    };
    Eventable::~Eventable()
    {
        std::for_each( mEvents.begin(), mEvents.end(), Deleter() );
    }

    void Eventable::fireEvent(const String& eventName, const ScriptVarList& inArgs, ScriptVarList& outArgs) const
    {
        EventList::const_iterator it = mEvents.find( eventName );
        if ( it != mEvents.end() ) {
            it->second->eval( inArgs, outArgs );
        }
    }

    void Eventable::removeEventHandler(const String& eventName)
    {
        EventList::iterator it = mEvents.find( eventName );
        if ( it != mEvents.end() )
        {
            delete it->second;
            mEvents.erase( it );
        }
    }
}// namespace Script
