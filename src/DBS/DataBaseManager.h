
#ifndef __DataBaseManager_H__
#define __DataBaseManager_H__

#include "Common/OgrePrerequisites.h"

class DataBase;
#include "Common/OgreSingleton.h"
#include "Common/OgreString.h"

using namespace Ogre;
typedef GeneralAllocatedObject		DataBaseAlloc;

class _OgreExport DataBaseManager : public Singleton< DataBaseManager >, public DataBaseAlloc
{
private:
	typedef map<String, DataBaseFactory*>::type	DataBaseFactoryMap;
    /// Зарегистрированные фабрики
    DataBaseFactoryMap  mDataBaseFactoryMap;

public:

    DataBaseManager();
    ~DataBaseManager();

    void registerFactory(const String& type, DataBaseFactory* factory);
    DataBaseFactory& getFactory(const String& type);
};


#endif
