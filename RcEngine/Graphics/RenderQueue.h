#ifndef RenderQueue_h__
#define RenderQueue_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

struct _ApiExport RenderQueueItem
{
	uint32_t Type;
	Renderable* Renderable;
	float SortKey;

	RenderQueueItem() {}
	RenderQueueItem( uint32_t type, class Renderable* rd, float key ) : Type(type), Renderable(rd), SortKey(key) { }
};
typedef std::vector<RenderQueueItem> RenderBucket;

class _ApiExport RenderQueue
{
public:
	enum Bucket
	{
		 /**
         * The renderer will try to find the optimal order for rendering all 
         * objects using this mode.
         * You should use this mode for most normal objects, except transparent
         * ones, as it could give a nice performance boost to your application.
         */
        BucketOpaque,
        
        /**
         * This is the mode you should use for object with
         * transparency in them. It will ensure the objects furthest away are
         * rendered first. That ensures when another transparent object is drawn on
         * top of previously drawn objects, you can see those (and the object drawn
         * using Opaque) through the transparent parts of the newly drawn
         * object. 
         */
        BucketTransparent,
        
        /**
         * A special mode used for rendering really far away, flat objects - 
         * e.g. skies. In this mode, the depth is set to infinity so 
         * spatials in this bucket will appear behind everything, the downside
         * to this bucket is that 3D objects will not be rendered correctly
         * due to lack of depth testing.
         */
        BucketSky,
        
        /**
         * A special mode used for rendering transparent objects that
         * should not be effected by {@link SceneProcessor}. 
         * Generally this would contain translucent objects, and
         * also objects that do not write to the depth buffer such as
         * particle emitters.
         */
        BucketTranslucent,
        
        /**
         * This is a special mode, for drawing 2D object
         * without perspective (such as GUI or HUD parts).
         * The spatial's world coordinate system has the range
         * of [0, 0, -1] to [Width, Height, 1] where Width/Height is
         * the resolution of the screen rendered to. Any spatials
         * outside of that range are culled.
         */
        BucketGui, 
	};

public:
	RenderQueue();
	~RenderQueue();

	void AddRenderBucket(uint32_t bucket);

	std::vector<RenderQueueItem>& GetRenderBucket(uint32_t bucket);
	std::map<uint32_t, RenderBucket*>& GetAllRenderBuckets() { return mRenderBuckets; }

	void AddToQueue(RenderQueueItem item, uint32_t bucket);
	void ClearAllQueue();


public:
	std::map<uint32_t, RenderBucket*> mRenderBuckets;
};


}


#endif // RenderQueue_h__
