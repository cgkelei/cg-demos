#ifndef SceneManager_h__
#define SceneManager_h__

//  [8/20/2012 Ruan]

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class Sky;
class SceneObject;

class _ApiExport SceneManager
{
public:
	typedef void (*ResTypeInitializationFunc)();
	typedef void (*ResTypeReleaseFunc)();
	typedef SceneObject* (*ResTypeFactoryFunc)( const String& name, const NameValuePairList* params);

	struct SceneObjectRegEntry
	{
		String					   typeString;
		ResTypeInitializationFunc  initializationFunc;  // Called when type is registered
		ResTypeReleaseFunc         releaseFunc;  // Called when type is unregistered
		ResTypeFactoryFunc         factoryFunc;  // Factory to create resource object
	};

public:
	SceneManager();
	virtual ~SceneManager();

	/**
	 * Register a scene object type.
	 */
	void RegisterType(uint32_t type, const String& typeString, ResTypeInitializationFunc inf,
		ResTypeReleaseFunc rf, ResTypeFactoryFunc ff );

	/**
	 * Get root scene node. if not created, create it.
	 */
	SceneNode* GetRootSceneNode();

	/**
	 * Create a new scene node, this will track the node in scene manager.
	 */
	SceneNode* CreateSceneNode( const String& name );

	/**
	 * Destroy a scene node, this will delete the scene node.
	 */
	void DestroySceneNode( SceneNode* node );


	SceneNode* FindSceneNode( const String& name ) const;

	Entity* CreateEntity( const String& entityName, const String& meshName, const String& groupName );
	Light* CreateLight( const String& name);

	void CreateSkyBox( const shared_ptr<Texture>& texture, bool cubemap = true, float distance = 100.0f );

	/**
	 * Update all scene graph node and transform.
	 */
	void UpdateSceneGraph(float delta);

	/**
	 * Update render queue, and remove scene node outside of the camera frustum.
	 */
	void UpdateRenderQueue(Camera* cam, RenderOrder order);

	RenderQueue* GetRenderQueue() { return mRenderQueue; }

	void RenderScene();

	vector<Light*>& GetSceneLight() { return mSceneLights; }

	AnimationController* GetAnimationController() const;

protected:
	void ClearScene();
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:
	// Registry of scene object types
	std::map< uint32_t, SceneObjectRegEntry >  mRegistry; 

	// scene object collection, first dimension is scene object type
	std::map< uint32_t, std::vector<SceneObject*> > mSceneObjectCollections;

	// Keep track of all scene node
    vector<SceneNode*> mAllSceneNodes;		// [0] is root node

	// keep track of light
	vector<Light*> mSceneLights;

	SceneNode* mSkyBoxNode;	
	Sky* mSkyBox;
	
	AnimationController* mAnimationController;

	RenderQueue* mRenderQueue;
};


}


#endif // SceneManager_h__

