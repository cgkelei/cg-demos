#ifndef SceneManager_h__
#define SceneManager_h__

//  [8/20/2012 Ruan]

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>

namespace RcEngine {

class SceneObject;
class Sky;

struct _ApiExport RenderQueueItem
{
	uint32_t Type;
	Renderable* Renderable;
	float SortKey;

	RenderQueueItem() {}
	RenderQueueItem( uint32_t type, class Renderable* rd, float key ) : Type(type), Renderable(rd), SortKey(key) { }
};
typedef vector<RenderQueueItem> RenderQueue;


class _ApiExport SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

	/**
	 * Get root scene node. if not created, create it.
	 */
	SceneNode* GetRootSceneNode();

	SceneNode* FindSceneNode( const String& name ) const;

	/**
	 * Create a new scene node, this will track the node in scene manager.
	 */
	SceneNode* CreateSceneNode( const String& name );

	/**
	 * Destroy a scene node, this will delete the scene node.
	 */
	void DestroySceneNode( SceneNode* node );

	Entity* CreateEntity( const String& name, const String& filePath );

	void CreateSkyBox( const shared_ptr<Texture>& texture, bool cubemap = true, float distance = 100.0f );

	/**
	 * Update all scene graph node and transform.
	 */
	void UpdateSceneGraph();

	/**
	 * Update render queue, and remove scene node outside of the camera frustum.
	 */
	void UpdateRenderQueue( Camera* cam );

	
	void AddToRenderQueue( SceneObject* so );

	void RenderScene();
	
	vector<RenderQueueItem>& GetRenderableQueue() { return mRendeQueue; }

protected:
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:

	/// Root scene node
	SceneNode* mSceneRoot;

	/// Keep track of all scene node
    vector<SceneNode*> mAllSceneNodes;

	SceneNode* mSkyBoxNode;	
	Sky* mSkyBox;

	vector<Entity*> mEntityLists;

	vector< RenderQueueItem > mRendeQueue;
};


}


#endif // SceneManager_h__

