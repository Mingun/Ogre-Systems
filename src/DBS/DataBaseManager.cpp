
#include "DBS/DataBase.h"

//#include "OLS/OgreLogManager.h"
#include "OES/OgreException.h"
//#include <algorithm>
using namespace Ogre;

//-----------------------------------------------------------------------
template<> DataBaseManager* Singleton<DataBaseManager>::ms_Singleton = 0;
DataBaseManager* DataBaseManager::getSingletonPtr(void)
{
    return ms_Singleton;
}
DataBaseManager& DataBaseManager::getSingleton(void)
{
    assert( ms_Singleton );  return ( *ms_Singleton );
}
//-----------------------------------------------------------------------
DataBaseManager::DataBaseManager()
{
}
//-----------------------------------------------------------------------
DataBaseManager::~DataBaseManager()
{
}
//-----------------------------------------------------------------------
void DataBaseManager::registerFactory(const String& type, DataBaseFactory* factory) 
{
    assert( factory );
    DataBaseFactoryMap::iterator it = mDataBaseFactoryMap.find( type );
    if ( it != mDataBaseFactoryMap.end() )
    {
        String msg("Database factory '")
        msg += type;
        msg += "' already exists. ";
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, msg, "DataBaseManager::registerFactory");
    }
    mDataBaseFactoryMap.insert( DataBaseFactoryMap::value_type( type, factory ) );
}
//-----------------------------------------------------------------------
DataBaseFactory& DataBaseManager::getFactory(const String& type)
{
    DataBaseFactoryMap::iterator it = mDataBaseFactoryMap.find( type );
    if ( it != nDataBaseFactoryMap.end() )
        return *(it->second);
    else
    {
        String msg("Database factory '")
        msg += type;
        msg += "' not found. ";
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, msg, "DataBaseManager::getFactory");
    }
}
//-----------------------------------------------------------------------