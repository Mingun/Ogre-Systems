
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
    /// ���������� ��� �������.
    virtual String      getName() const = 0;
    /// ���������� ������ type_info, ��������������� � C++ ����� �������.
    virtual const std::type_info& getType() const = 0;
    /// ���������� ������ �������.
    virtual int         getSize() const = 0;
    /// ���������� �������� �������.
    virtual int         getPrecision() const = 0;
    /// ���������� true, ���� ������� ����� ����� null ��������.
    virtual bool        isNullable() const = 0;
    /// ���������� ����� �������.
    virtual int         getNumber() const = 0;
};

#endif
