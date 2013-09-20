#ifndef ResourceManager_h__
#define ResourceManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <Resource/Resource.h>

namespace RcEngine {

class Resource;

class _ApiExport ResourceManager : public Singleton<ResourceManager>
{
public:
	SINGLETON_DECL_HEADER(ResourceManager);

public:
	typedef void (*ResTypeInitializationFunc)();
	typedef void (*ResTypeReleaseFunc)();
	typedef shared_ptr<Resource> (*ResTypeFactoryFunc)( ResourceManager*, ResourceHandle, const String&, const String&);

	struct ResourceRegEntry
	{
		String					   TypeString;
		ResTypeFactoryFunc         FactoryFunc;  // Factory to create resource object
	};


	struct _ApiExport ResourceGroup
	{
		ResourceGroup() : MemoryBudget(0), MemoryUse(0) {}

		unsigned MemoryBudget;

		unsigned MemoryUse;
		unordered_map<String, shared_ptr<Resource> > Resources;
	};


public:
	ResourceManager();
	~ ResourceManager();

	void RegisterType(uint32_t type, const String& typeString, ResTypeFactoryFunc factoryFunc);

	void AddResourceGroup(const String& groupName);

	ResourceHandle AddResource(uint32_t type, const String& name, const String& group);
	shared_ptr<Resource> GetResourceByHandle( ResourceHandle handle );
	shared_ptr<Resource> GetResourceByName(uint32_t type, const String& name, const String& group );

	void LoadAllFromDisk();

protected:
	ResourceHandle AddNonExitingResource(uint32_t type, const String& name, const String& group);
	ResourceHandle GetNextHandle();  

protected:	
	uint32_t mNextHandle;
	std::map<int, ResourceRegEntry>  mRegistry;  // Registry of resource type
	std::map<ResourceHandle, shared_ptr<Resource> > mResourcesByHandle;
	unordered_map<String, ResourceGroup> mResourcesWithGroup;
	
};



}


#endif // ResourceManager_h__
