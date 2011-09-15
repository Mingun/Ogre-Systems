
#ifndef __DataBaseFactory_H__
#define __DataBaseFactory_H__

#include "Common/OgrePrerequisites.h"

#include "DBS/DataBase.h"
#include "Common/OgreFactoryObj.h"

using namespace Ogre;
typedef GeneralAllocatedObject      DataBaseAlloc;

class DataBaseServer;

/** Абстрактный класс фабрики, плагины баз данных могут зарегистрировать
    его конкретные подклассы.
    @remarks
        Весь доступ к базам данных управляется через абстрактный класс
        DataBase. Плагины должны предоставить реализацию инфраструктуры как 
        самой базы данных, так и реализацию фабрики, которая будет ее создавать.
    @par
        Плагины поддержки баз данных создают подкласс DataBase И 
        подкласс DataBaseFactory, который будет создавать инстанции подкласса 
        DataBaseServer. Смотрите плагины 'MySQL' и 'ODBS' для примера.
*/
class _OgreExport DataBaseFactory : public FactoryObj< DataBaseServer >, public DataBaseAlloc
{
public:
    virtual ~DataBaseFactory() {}

    DataBase* createDatabase(const String& name) = 0;
    DataBase* openDatabase(const String& name) = 0;
};


#endif
