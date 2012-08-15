#ifndef ModuleManager_h__
#define ModuleManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <Core/DynLib.h>

namespace RcEngine{
namespace Core{

class ModuleInfo
{
public:
	ModuleInfo() {};
	~ModuleInfo() {};

private:
	//bool operator == (const ModuleInfo &other) const;
	ModuleInfo &operator = (const ModuleInfo &other);
	ModuleInfo(const ModuleInfo &other);
		
public:
	String mModuleName;
	IModule* mModuleSystem;
	ModuleType mModuleType;
	DynLib* mDynLib;
};

class _ApiExport ModuleManager : public Singleton<ModuleManager>
{
public:
	SINGLETON_DECL_HEADER(ModuleManager)

public:
	ModuleManager(void);
	~ModuleManager(void);

	bool HasMoudle(const String& name);
	bool HasModule(ModuleType modType);

	bool Load(const String& name, ModuleType modType);
	bool Load(ModuleType modType);

	bool Unload(const String& name);
	bool Unload(ModuleType modType);

	bool LoadAll();
	bool UnloadAll();

	IModule* GetModuleByName(const String &strModule);
	IModule* GetMoudleByType(ModuleType modType);

private:
	ModuleInfo* GetMoudleInfoByName(const String &strModule);
	ModuleInfo* GetMoudleInfoByType(ModuleType modType);
	void Unload(ModuleInfo* module);

private:
	std::vector<ModuleInfo*> mMoudles;
};


} // Namespace Core
} // Namespace RcEngine

#endif // ModuleManager_h__