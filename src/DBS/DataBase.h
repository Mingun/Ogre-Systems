
#ifndef __DataBase_H__
#define __DataBase_H__

#include "Common/OgrePrerequisites.h"
#include "Common/OgreString.h"
#include "Common/OgreSharedPtr.h"

using namespace Ogre;
typedef GeneralAllocatedObject      DataBaseAlloc;

class DataBaseResultSet;
class DataBaseTable;

class _OgreExport DataBase : public DataBaseAlloc
{
public:
    typedef SharedPtr< DataBaseResultSet >    DataBaseResultSetPtr;
    typedef SharedPtr< DataBaseTable >        DataBaseTablePtr;
    typedef vector< DataBaseTablePtr >::type  DataBaseTableList;

protected:
    String          mDBName;

public:

    OGRE_AUTO_MUTEX // public to allow external locking
    /**
    @remarks
        Usual constructor - called by DataBaseServer.
    */
    DataBase();

    /**
    @remarks
    Default destructor.
    */
    virtual ~DataBase() {}

    /// Возвращает имя базы данных
    const String& getName() const { return mDBName; }

    virtual bool open(const String& dbName) = 0;
    virtual bool open(const String& dbName, const String& user) = 0;
    virtual bool open(const String& dbName, const String& user, const String& password) = 0;
    virtual bool isOpen() const = 0;
    
    virtual DataBaseResultSetPtr  query(const String& sql) = 0;
    
    virtual DataBaseTablePtr      createTable(const String& name) = 0;
    virtual bool                  dropTable(const String& name) = 0;
    virtual bool                  dropTable(const DataBaseTable* table) = 0;
    virtual DataBaseTableList     listTables() = 0;
};

#endif
