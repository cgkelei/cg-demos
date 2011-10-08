#include "Resource.h"
#include "Exception.h"
#include "LogManager.h"

namespace Engine
{

	Resource::Resource()
		: mManager(0), mHandle(0),mIsBackgroundLoaded(false), mLoadingState(RS_Unloaded),
		  mLoader(0), mSize(0)
	{
		
	}

	Resource::Resource( ResourceManager* manager, const String& name, ResourceHandle handle, ResourceLoader* loader /*= 0*/ )
		: mManager(manager), mName(name), mHandle(handle), mLoader(loader), mSize(0),
		 mRefCount(0),mIsBackgroundLoaded(false), mLoadingState(RS_Unloaded)
	{

	}

	Resource::~Resource()
	{

	}

	bool Resource::Load( bool backgroundThread/*=false*/ )
	{
		if(IsManaged())
		{

		}
		return true;
	}

	bool Resource::Reload( void )
	{
		return true;
	}

	bool Resource::IsLoading() const
	{
		return GetLoadingState() == RS_Loading;
	}

	bool Resource::IsLoaded() const
	{
		return GetLoadingState() == RS_Loaded;
	}

	ResourceState Resource::GetLoadingState() const 
	{
		return mLoadingState;
	}

	void Resource::SetLoadingState( ResourceState state )
	{
		mLoadingState = state;
	}

	bool Resource::Unload( void )
	{
		return true;
	}

	void Resource::IncRef()
	{
		if(IsManaged())
		{
			mRefCount++;
		}
	}

	void Resource::DecRef()
	{
		if(IsManaged())
		{
			mRefCount--;
		}

	}
}