#ifndef __USSScriptVarList_H__
#define __USSScriptVarList_H__

#pragma once
#include "Common/OgrePrerequisites.h"
#include "USS/USSPrerequisites.h"

#include "Common/OgreString.h"

namespace Script {
    class _ScriptExport ScriptVarList : public Ogre::vector< ScriptVar* >::type
    {
        typedef Ogre::vector< ScriptVar* >::type    _Base;
        typedef ScriptVarList                       _Self;
    public:
        static ScriptVarList BLANK;
    public:
        ScriptVarList() : _Base() {}
        ~ScriptVarList();
        ScriptVarList(const ScriptVarList& rhs);

        ScriptVarList* clone() const { return new _Self(*this); }
        void pack(Bridge& bridge) const;
        void unpack(Bridge& bridge) const;
        int check(Bridge& bridge) const;

        inline void add(ScriptVar* var) { push_back( var ); }
        String toString(const String& delim) const;
        String typeName(const String& delim) const;
        
        inline reference operator[](size_type pos)
        { return _Base::operator[](pos); }
        inline const_reference operator[](size_type pos) const
        { return _Base::operator[](pos); }
        /// Получает переменную по ее имени
        reference operator[](const String& name);
        const_reference operator[](const String& name) const;
    };

    template<class OStream>
    OStream& operator <<(OStream& os, const ScriptVarList& vars)
    {
        return os << vars.toString(", ");
    }

}// namespace Script

#endif // __USSScriptVarList_H__
