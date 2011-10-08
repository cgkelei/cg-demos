#pragma once
#ifndef _ResourceManager__H
#define _ResourceManager__H

#include "Prerequisites.h"
#include "Resource.h"

namespace Engine
{
	typedef std::pair<String, Resource*> PairResourceName;
	typedef std::pair<ResourceHandle, Resource*> PairResourceHandle;


	class _ApiExport ResourceManager
	{
	public:
		ResourceManager();
		virtual ~ResourceManager();

		void ReloadAll();
		void UnloadAll();
		void RemoveAll();
		void LoadAll();
		
		Resource* CreateResource(const String& name, const String& origin, Resource* pRes, ResourceLoader* loader);
		
		void RemoveResource(const String& name);
		void RemoveResource(ResourceHandle handle);

		Resource* GetResource(const String& name);
		Resource* GetResource(ResourceHandle handle);

		void NotifyResourceLoaded(Resource* res);
		void NotifyResourceUnloaded(Resource* res);

		virtual bool Load(ResourceHandle handle, bool background);
		virtual bool Load(const String& name, bool background);
		virtual bool Unload(ResourceHandle handle);
		virtual bool Unload(const String& name);

	protected:
		/** Allocates the next handle. */
		ResourceHandle GetNextHandle(void);

		void AddImpl( Resource* res );
		void RemoveImpl( Resource* res );


	protected:
		typedef std::map<ResourceHandle, Resource*> ResourceHandleMap;
		typedef ResourceHandleMap::iterator ResourceHandleMapIter;
		typedef std::map<String, Resource*> ResourceNameMap;
		typedef ResourceNameMap::iterator ResourceNameMapIter;

#pragma warning(disable : 4251)

		ResourceHandleMap mResMapByHandle;
		ResourceNameMap mResMapByName;
		ResourceHandle mNextHandle;

#pragma warning(default : 4251)
	};

}


#endif