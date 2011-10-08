#include "ResourceManager.h"
#include "LogManager.h"

namespace Engine
{
	ResourceManager::ResourceManager()
	{

	}

	ResourceManager::~ResourceManager()
	{

	}

	void ResourceManager::ReloadAll()
	{

	}

	void ResourceManager::UnloadAll()
	{

	}

	void ResourceManager::RemoveAll()
	{

	}

	void ResourceManager::LoadAll()
	{

	}

	Resource* ResourceManager::CreateResource( const String& name, const String& origin, Resource* pRes, ResourceLoader* loader )
	{
		return NULL;
	}

	void ResourceManager::RemoveResource( const String& name )
	{

	}

	void ResourceManager::RemoveResource( ResourceHandle handle )
	{

	}

	Resource* ResourceManager::GetResource( const String& name )
	{
		ResourceNameMapIter it = mResMapByName.find(name);
		if (it != mResMapByName.end())
		{
			return it->second;
		}
		else
		{
			LogMessageStream << "Can't find resource, name: " << name << std::endl;
			return NULL;
		}
	}

	Resource* ResourceManager::GetResource( ResourceHandle handle )
	{
		ResourceHandleMapIter it = mResMapByHandle.find(handle);
		if (it != mResMapByHandle.end())
		{
			return it->second;
		}
		else
		{
			LogMessageStream << "Can't find resource" << std::endl;
			return NULL;
		}
	}

	Engine::ResourceHandle ResourceManager::GetNextHandle( void )
	{
		return mNextHandle++;
	}

	void ResourceManager::NotifyResourceLoaded( Resource* res )
	{
		
	}

	void ResourceManager::NotifyResourceUnloaded( Resource* res )
	{

	}

	bool ResourceManager::Load( ResourceHandle handle,bool background )
	{
		Resource* res = GetResource(handle);
		if(res != NULL)
			return res->Load(background);
	
		return false;
	}

	bool ResourceManager::Load( const String& name,bool background )
	{
		Resource* res = GetResource(name);
		if(res != NULL)
			return res->Load(background);

		return false;
	}

	bool ResourceManager::Unload( ResourceHandle handle )
	{
		return false;
	}

	bool ResourceManager::Unload( const String& name )
	{
		return false;
	}
}