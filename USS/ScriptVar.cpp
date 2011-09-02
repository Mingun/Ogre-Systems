
#include "USS/ScriptVar.h"
#include "USS/IScriptable.h"
#include <algorithm>

namespace Script {

    String BoolVar::toString() const
    {
        StringStream ss;
        ss << mValue;
        return ss.str();
    }

    String IntVar::toString() const
    {
        StringStream ss;
        ss << mValue;
        return ss.str();
    }

    String RealVar::toString() const
    {
        StringStream ss;
        ss << mValue;
        return ss.str();
    }

    String CharVar::toString() const
    {
        StringStream ss;
        ss << mValue;
        return ss.str();
    }

    String StringVar::toString() const
    {
        return mValue;
    }

    String IteratorVar::toString() const
    {
        StringStream ss;
        ss << mValue;
        return ss.str();
    }

    String ScriptableVar::toString() const
    {
        return mValue->toString();
    }

    String HashVar::toString() const
    {
        StringStream ss;
        ss << "HashVar(size=" << mValue.size() << ")";
        return ss.str();
    }

    String FunctionVar::toString() const
    {
        return "FunctionVar";
    }

    //=============================================================================
    Hash::Hash(const Hash& rhs)
    {
        this->~Hash();
        for ( const_iterator it = rhs.begin(); it != rhs.end(); ++it )
            add( it->first, it->second->clone() );
    }

    Hash::~Hash()
    {
        for ( iterator it = begin(); it != end(); ++it )
        {
            delete it->second;
            it->second = NULL;
        }
        clear();
    }

}// namespace Script;
