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

#include "OPS/OgrePlugin.h"

class PLUGIN_API Plugin : public Ogre::Plugin {
	static Ogre::String mPluginName;

public:
	Plugin();
	~Plugin();
	virtual const Ogre::String& getName() const;

	virtual void install();
	virtual void initialise();
	virtual void shutdown();
	virtual void uninstall();
};

extern "C" PLUGIN_API void dllStartPlugin(void);
extern "C" PLUGIN_API void dllStopPlugin(void);
