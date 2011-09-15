
#ifndef __DataBaseColumn_H__
#define __DataBaseColumn_H__

#include "Common/OgrePrerequisites.h"
#include "Common/OgreString.h"

using namespace Ogre;
typedef GeneralAllocatedObject      DataBaseAlloc;

class _OgreExport DataBaseColumn : public DataBaseAlloc
{
public:
    virtual ~DataBaseColumn() {}
    /// Возвращает имя столбца.
    virtual String      getName() const = 0;
    /// Возвращает объект type_info, ассоциированный с C++ типом столбца.
    virtual const std::type_info& getType() const = 0;
    /// Возвращает размер столбца.
    virtual int         getSize() const = 0;
    /// Возвращает точность столбца.
    virtual int         getPrecision() const = 0;
    /// Возвращает true, если столбец может иметь null значения.
    virtual bool        isNullable() const = 0;
    /// Возвращает номер столбца.
    virtual int         getNumber() const = 0;
};

#endif
