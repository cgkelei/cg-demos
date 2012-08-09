#include "Core/ModuleManager.h"
#include "Core/Utility.h"
#include "Core/IModule.h"

namespace RcEngine{

	SINGLETON_DECL(Core::ModuleManager)

		namespace Core{

			ModuleManager::ModuleManager(void)
			{
			}

			ModuleManager::~ModuleManager(void)
			{
				UnloadAll();
				mMoudles.clear();
			}


			bool ModuleManager::Load(const String& name, ModuleType modType)
			{
				ModuleInfo* info = GetMoudleInfoByName(name);
				if(!info)
				{
					info = new ModuleInfo();
					info->mModuleName = name;
					info->mModuleType = modType;
					info->mDynLib = new DynLib(name);
					DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)info->mDynLib->GetSymbol("dllStartPlugin");
					pFunc(&info->mModuleSystem);
					mMoudles.push_back(info);
				}
				return true;
			}

			bool ModuleManager::Load( ModuleType modType )
			{
				ModuleInfo* info = GetMoudleInfoByType(modType);
				if(!info)
				{
					info = new ModuleInfo();
					info->mModuleName = ModuleNames[modType];
					info->mModuleType = modType;
					info->mDynLib = new DynLib(ModuleNames[modType]);
					info->mDynLib->Load();
					DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)info->mDynLib->GetSymbol("dllStartPlugin");
					pFunc(&info->mModuleSystem);
					mMoudles.push_back(info);
				}
				return true;
			}

			bool ModuleManager::Unload( const String& name )
			{
				return true;
			}

			bool ModuleManager::Unload( ModuleType modType )
			{
				return true;
			}

			bool ModuleManager::LoadAll()
			{
				return true;
			}

			bool ModuleManager::UnloadAll()
			{
				for (auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					(*iter)->mModuleSystem->Shutdown();
					(*iter)->mDynLib->Unload();
					delete (*iter)->mModuleSystem;
					delete (*iter)->mDynLib;
					delete (*iter);
				}
				return true;
			}

			bool ModuleManager::HasMoudle( const String& strModule )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleName == strModule)
						return true;
				}
				return false;
			}

			bool ModuleManager::HasModule( ModuleType modType )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleType == modType)
						return true;
				}
				return false;
			}

			IModule* ModuleManager::GetModuleByName( const String &strModule )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleName == strModule)
						return (*iter)->mModuleSystem;
				}
				return 0;
			}

			IModule* ModuleManager::GetMoudleByType( ModuleType modType )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleType == modType)
						return (*iter)->mModuleSystem;
				}
				return 0;
			}

			ModuleInfo* ModuleManager::GetMoudleInfoByName( const String &strModule )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleName == strModule)
						return (*iter);
				}
				return 0;
			}

			ModuleInfo* ModuleManager::GetMoudleInfoByType( ModuleType modType )
			{
				for(auto iter = mMoudles.begin(); iter != mMoudles.end(); ++iter)
				{
					if((*iter)->mModuleType == modType)
						return (*iter);
				}
				return 0;
			}

	} // Namespace Core
} // Namespace RcEngine