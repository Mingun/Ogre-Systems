
#include "USS/ScriptVarList.h"
#include "USS/ScriptVar.h"
#include <algorithm>

namespace Script {

    ScriptVarList ScriptVarList::BLANK = ScriptVarList();

    ScriptVarList::ScriptVarList(const ScriptVarList& rhs)
    {
        this->~ScriptVarList();
        for ( const_iterator it = rhs.begin(); it != rhs.end(); ++it )
        {
            assert( *it );
            push_back( (*it)->clone() );
        }
    }
    class Deleter
    {
    public:
        template<typename T>
        void operator()(const T* p) const
        {
            delete p;
            p = NULL;
        }
    };
    ScriptVarList::~ScriptVarList()
    {
        std::for_each( begin(), end(), Deleter() );
        clear();
    }

    void ScriptVarList::pack(Bridge& bridge) const 
    {
        for ( const_iterator it = begin(); it != end(); ++it )
        {
            (*it)->pack( bridge );
            bridge.next();
        }
    }

    void ScriptVarList::unpack(Bridge& bridge) const 
    {
        for ( const_iterator it = begin(); it != end(); ++it )
        {
            (*it)->unpack( bridge );
            bridge.next();
        }
    }

    int ScriptVarList::check(Bridge& bridge) const 
    {
        if ( !bridge.check( *this ) )
            return false;
        bridge.next();
        for ( const_iterator it = begin(); it != end(); ++it )
        {
            if ( !(*it)->check( bridge ) )
                return false;
            bridge.next();
        }
        return true;
    }

    String ScriptVarList::toString(const String& delim) const 
    {
        StringStream ss;
        const_iterator it = begin();
        if ( it != end() )
            ss << (*it++)->toString();
        for ( ; it != end(); ++it )
        {
            ss << delim << (*it)->toString();
        }
        return ss.str();
    }

    String ScriptVarList::typeName(const String& delim) const 
    {
        StringStream ss;
        const_iterator it = begin();
        if ( it != end() )
            ss << (*it++)->typeName();
        for ( ; it != end(); ++it )
        {
            ss << delim << (*it)->typeName();
        }
        return ss.str();
    }

    ScriptVarList::reference ScriptVarList::operator[](const String& name)
    {
        for ( iterator it = begin(); it != end(); ++it )
        {
            if ( (*it)->name() == name )
                return *it;
        }
        static value_type Null = NULL;
        return Null;
    }
    ScriptVarList::const_reference ScriptVarList::operator[](const String& name) const
    {
        for ( const_iterator it = begin(); it != end(); ++it )
        {
            if ( (*it)->name() == name )
                return *it;
        }
        static value_type Null = NULL;
        return Null;
    }

}// namespace Script
