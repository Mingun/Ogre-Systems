
#ifndef __DataBaseServer_H__
#define __DataBaseServer_H__

#include "Common/OgrePrerequisites.h"
#include "Common/OgreString.h"

using namespace Ogre;
typedef GeneralAllocatedObject      DataBaseAlloc;


class _OgreExport DataBaseServer : public DataBaseAlloc
{
public:
    typedef vector< DataBase* >::type   DataBaseList;
protected:
    /// Адрес сервера базы данных (без порта)
    String          mAddress;
    /// Порт сервера базы данных
    int             mPort;

public:

    OGRE_AUTO_MUTEX // public to allow external locking
    /**
    @remarks
        Usual constructor - called by DataBaseFactory.
    */
    DataBaseServer();

    /**
    @remarks
    Обычный деструктор.
    */
    virtual ~DataBaseServer() {}

    /// Возвращает строку, описывающую тип сервера базы данных (например, 'MySQL').
    const String& getType() const = 0;
    
    /// Возвращает версию сервера базы данных.
    int getVersion() const = 0;
    
    /// Возвращает адрес сервера базы данных
    const String& getAddress() const { return mAddress; }
    
    /// Возвращает порт сервера базы данных
    int getPort() const { return mPort; }

    /**
    Выполняет запрос на сервере базы данных.
    */
    virtual void        query(const String& sql) = 0;
    virtual DataBase*   createDatabase(const String& name) = 0;
    virtual DataBase*   openDatabase(const String& name) = 0;
    virtual bool        dropDatabase(const String& name) = 0;
    virtual bool        dropDatabase(DataBase* db) = 0;
    virtual DataBaseList listDatabase() = 0;
};

#endif
