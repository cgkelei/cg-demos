#include <Graphics/RenderQueue.h>
#include <Core/Exception.h>

namespace RcEngine {

RenderQueue::RenderQueue()
{
	// set up default bucket
	mRenderBuckets.insert(std::make_pair(BucketOpaque, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketTransparent, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketBackground, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketTranslucent, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketOverlay, new RenderBucket()));
}

RenderQueue::~RenderQueue()
{
	for (auto& kv : mRenderBuckets)
		delete kv.second;
}

RenderBucket& RenderQueue::GetRenderBucket( Bucket bucket )
{
	if (mRenderBuckets.find(bucket) == mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Render bucket not exits!",  "RenderQueue::GetRenderBucket");
	}

	return (*mRenderBuckets[bucket]);
}

void RenderQueue::AddToQueue( RenderQueueItem item, Bucket bucket )
{
	if (mRenderBuckets.find(bucket) == mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Render bucket not exits!",  "RenderQueue::AddToQueue");
	}

	mRenderBuckets[bucket]->push_back(item);
}

void RenderQueue::AddRenderBucket( Bucket bucket )
{
	if (mRenderBuckets.find(bucket) != mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Render bucket already exits!",  "RenderQueue::AddRenderBucket");
	}

	mRenderBuckets.insert(std::make_pair(bucket, new RenderBucket()));
}

void RenderQueue::ClearAllQueue()
{
	for (auto iter = mRenderBuckets.begin(); iter != mRenderBuckets.end(); ++iter)
	{
		iter->second->clear();
	}
}

void RenderQueue::ClearQueue( Bucket bucket )
{
	mRenderBuckets[bucket]->clear();
}




}
