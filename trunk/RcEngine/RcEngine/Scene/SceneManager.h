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

	Entity* CreateEntity( const String& entityName, const String& meshName, const String& groupName );

	void CreateSkyBox( const shared_ptr<Texture>& texture, bool cubemap = true, float distance = 100.0f );

	/**
	 * Update all scene graph node and transform.
	 */
	void UpdateSceneGraph(float delta);

	/**
	 * Update render queue, and remove scene node outside of the camera frustum.
	 */
	void UpdateRenderQueue( Camera* cam );

	void RenderScene();
	
	vector<RenderQueueItem>& GetRenderQueue() { return mRendeQueue; }

	AnimationController* GetAnimationController() const;

protected:
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:

	/// Root scene node
	SceneNode* mSceneRoot;

	AnimationController* mAnimationController;

	/// Keep track of all scene node
    vector<SceneNode*> mAllSceneNodes;

	SceneNode* mSkyBoxNode;	
	Sky* mSkyBox;

	vector<Entity*> mEntityLists;

	vector< RenderQueueItem > mRendeQueue;
};


}


#endif // SceneManager_h__

