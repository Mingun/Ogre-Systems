// ��������� ���� ifdef - ����������� ���� �������� �������, ������� �������� ������� 
// �� DLL. ��� ����� � �������� ���� DLL ��������������� � �������� PLUGIN_EXPORTS,
// ������������ � ��������� ������. ���� ������ �� ������ ����������� � ����� �������,
// ������������ ��� DLL. ���� ����� ����� ������ ������, ��� �������� ����� �������� 
// ���� ����, ����� PLUGIN_API ������� ��� ��������������� �� DLL, � �� �����, ���
// DLL ����� �������, ������������ ���� ��������, ��� ��������������.
#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif

#include "OMS/OgreSharedPtr.h"
#include "OPS/OgrePlugin.h"

class LuaScriptEngine;

namespace Extension {

class PLUGIN_API Plugin : public Ogre::Plugin {
	static Ogre::String mPluginName;

    Ogre::SharedPtr<LuaScriptEngine> luaEngine;
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
