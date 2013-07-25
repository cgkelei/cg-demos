#ifndef Resource_h__
#define Resource_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class ResourceManager;

typedef uint32_t ResourceHandle;

enum ResourceTypes
{
	RT_Undefined = 0,
	RT_Mesh,
	RT_Material,
	RT_Effect,
	RT_Animation,
	RT_Texture,
	RT_Pipeline,
	RT_Font
};

class _ApiExport Resource
{
public:
	enum LoadState
	{
		Unloaded,
		Loading,
		Loaded,
		Unloading,
	};

public:
	Resource(ResourceTypes resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	virtual ~Resource();

	virtual shared_ptr<Resource> Clone();

	const String& GetName() const				{ return mName; }
	const String& GetResourceGroup() const		{ return mGroup; }
	ResourceHandle GetResourceHandle() const    { return mResourceHandle; }
	uint32_t GetSize() const					{ return mSize; }
	uint32_t GetResourceType() const			{ return mResourceType; }
	
	void Load(bool background = false);
	void Unload();
	void Reload();
	void Touch();

	void SetLoadState(LoadState state);
	LoadState GetLoadState();

	bool IsLoaded() const						{ return mLoadState == Loaded; }
	bool IsLoading() const						{ return mLoadState == Loading; }

private:
	void LoadSync();
	void LoadAsync();

protected:
	
	virtual void LoadImpl() = 0;
	virtual void UnloadImpl() = 0;


protected:
	ResourceManager* mCreator;
	String mName;
	String mGroup;
	bool mBackground;
	LoadState mLoadState;
	uint32_t mSize;

	ResourceHandle mResourceHandle;
	ResourceTypes mResourceType;

	fast_mutex mMutex; 
};

}


#endif // Resource_h__
