#include <Resource/Resource.h>


namespace RcEngine {

Resource::Resource( uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: mResourceType(resType), mCreator(creator), mName(name), mResourceHandle(handle), mGroup(group), mBackground(false), mSize(0),
		mLoadState(Unloaded)
{

}

Resource::~Resource()
{

}

void Resource::Load( bool background /*= false*/ )
{
	LoadSync();
}

void Resource::Unload()
{

}

void Resource::Reload()
{

}

void Resource::Touch()
{

}

void Resource::LoadSync()
{
	if (mLoadState == Unloaded)
	{
		SetLoadState(Loading);
		LoadImpl();
		SetLoadState(Loaded);
	}
}

void Resource::LoadAsync()
{

}

Resource::LoadState Resource::GetLoadState()
{
	LoadState state;
	
	mMutex.lock();
	state = mLoadState;
	mMutex.unlock();

	return state;
}

void Resource::SetLoadState( LoadState state )
{
	mMutex.lock();
	mLoadState = state;
	mMutex.unlock();
}

}