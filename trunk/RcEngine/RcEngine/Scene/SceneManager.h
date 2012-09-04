#ifndef SceneManager_h__
#define SceneManager_h__

//  [8/20/2012 Ruan]

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>

namespace RcEngine {

class SceneObject;
class SkyBox;

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

	SkyBox* CreateSkyBox( const shared_ptr<Texture>& frontTex, const shared_ptr<Texture>& backTex,
		const shared_ptr<Texture>& leftTex, const shared_ptr<Texture>& rightTex, const shared_ptr<Texture>& topTex,
		const shared_ptr<Texture>& bottomTex, float distance = 100.0f );

	SkyBox* CreateSkyBox( const shared_ptr<Texture>& cubicTex, float distance = 100.0f );

	void UpdateSceneGraph();

	void UpdateRenderQueue( Camera* cam );

	void AddToRenderQueue( SceneObject* so );

	void RenderScene();

protected:
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:

	/// Root scene node
	SceneNode* mSceneRoot;

	/// Keep track of all scene node
    vector<SceneNode*> mAllSceneNodes;

	SceneNode* mSkyBoxNode;	
	SkyBox* mSkyBox;

	vector<Entity*> mEntityLists;

	vector< RenderQueueItem > mRendeQueue;
};


}


#endif // SceneManager_h__

