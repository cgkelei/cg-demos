#pragma once
#ifndef _Resource__H
#define _Resource__H

#include "Prerequisites.h"

namespace Engine
{
	typedef unsigned int ResourceHandle;

	enum ResourceState
	{
		/// Not loaded
		RS_Unloaded,

		/// Loading is in progress
		RS_Loading,

		/// Fully loaded
		RS_Loaded,

		/// Currently unloading
		RS_Unloading,

		/// Fully prepared
		RS_Prepared,

		/// Preparing is in progress
		RS_Preparing
	};

	class _ApiExport Resource
	{
	public:
		Resource(ResourceManager* manager, const String& name, ResourceHandle handle, ResourceLoader* loader = 0);
		virtual ~Resource();
		
		

		bool Load (bool backgroundThread=false);
		bool Unload (void);
		bool Reload (void);
		
		String GetName() const									{ return mName; }
		String GetOrigin() const								{ return mOrigin; }
		uint32 GetSize() const									{ return mSize; }

		void NotifyOrigin(const String& origin)					{ mOrigin = origin; }

		bool IsLoading() const;							
		bool IsLoaded() const;
		ResourceState GetLoadingState() const;
		void SetLoadingState(ResourceState state);

		bool IsBackgroundLoaded(void) const						{ return mIsBackgroundLoaded; }
		void SetBackgroundLoaded(bool bl)						{ mIsBackgroundLoaded = bl; }
	
		ResourceManager* GetResourceManager()					{ return mManager; }
		const ResourceManager* GetResourceManager() const		{ return mManager; }
		
		bool IsManaged() const									{ return mManager != NULL; }
		
		uint32 GetRefCount() const								{ return mRefCount; }
		void IncRef();
		void DecRef();

	protected:
		Resource();

		/// ��μ�����Դ����������
		virtual bool LoadImpl() = 0;
		virtual bool UnloadImpl() = 0;
		virtual uint32 CalculateSize() const  = 0;
		

	protected:

		ResourceManager* mManager;
		ResourceLoader* mLoader;
		
		ResourceState mLoadingState;
		
		String mName;		/// ��Դ�����ƣ�һ��ҪΨһ��
		String mOrigin;		/// ��Դ��·��
		
		ResourceHandle mHandle;

		bool mIsBackgroundLoaded;
		uint32 mRefCount;
		uint32 mSize;
	
	};


	class _ApiExport ResourceLoader
	{
	public:
		ResourceLoader(){}
		virtual ~ResourceLoader() {}

		virtual bool LoadResource(Resource* pRes) = 0;
	};
}


#endif