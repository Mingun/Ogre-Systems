
#ifndef __DataBaseResultSet_H__
#define __DataBaseResultSet_H__

#include "Common/OgrePrerequisites.h"
#include "Common/OgreString.h"
#include "Common/OgreSharedPtr.h"

using namespace Ogre;
typedef GeneralAllocatedObject      DataBaseAlloc;
class DataBaseColumn;
class DataBaseRow;

class _OgreExport DataBaseResultSet : public DataBaseAlloc
{
public:
    typedef SharedPtr< DataBaseColumn >         DataBaseColumnPtr;
    typedef ConstVectorIterator< DataBaseRow >  DataBaseRowIterator;

    virtual ~DataBaseResultSet() {}

    //! Returns the number of columns returned by the last query executed by the
    //! associated statement.
    int getColumnCount() const = 0;

    /// Возвращает описание 'i'-го столбца в возвращенном наборе. (индекс отсчитывается с 0)
    DataBaseColumnPtr operator[](int i) const = 0;
    
    /// Возвращает описание столбца с именем 'name' в возвращенном наборе.
    DataBaseColumnPtr operator[](std::string const & name) const = 0;
    
    /// \brief Возвращает итератор на первую строчку в наборе результатов.
    /// Разыминовывание итератора возвращает инстанцию класса DataBaseRow.
    DataBaseRowIterator iterator() const = 0;
    // iterator begin() const = 0;
    /// Returns an iterator to one past the last row in the result set.
    // iterator end() const = 0;

};

#endif
