// Следующий блок ifdef - стандартный путь создания макроса, который упростит экспорт 
// из DLL. Все файлы в пределах этой DLL откомпилированы с символом PLUGIN_EXPORTS,
// определенном в командной строке. Этот символ не должен определятся в любом проэкте,
// использующем эту DLL. Этим путем любой другой проэкт, чьи исходные файлы включают 
// этот файл, видят PLUGIN_API функции как импортированные из DLL, в то время, как
// DLL видят символы, определенные этой командой, как экспортируемые.
#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif

#include "OPS/OgrePlugin.h"
#include "OMS/OgreSharedPtr.h"
#include "USS/USSPrerequisites.h"

namespace test {
class Pair;
class PairObject;
class PairClass;
}

namespace Extension {

class PLUGIN_API Plugin : public Ogre::Plugin {
	static Ogre::String mPluginName;

    // классы
    Ogre::SharedPtr<test::PairClass>  mClass;
    // объекты
    Ogre::SharedPtr<test::PairObject> mObj;
public:
	Plugin();
	~Plugin();
	virtual const Ogre::String& getName() const;

	virtual void install();
	virtual void initialise();
	virtual void shutdown();
	virtual void uninstall();
};

}// namespace Extension

extern "C" PLUGIN_API void dllStartPlugin(void);
extern "C" PLUGIN_API void dllStopPlugin(void);
