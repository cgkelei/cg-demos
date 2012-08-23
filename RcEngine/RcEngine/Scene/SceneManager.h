#ifndef SceneManager_h__
#define SceneManager_h__

//  [8/20/2012 Ruan]

#include <Core/Prerequisites.h>

namespace RcEngine {


class _ApiExport SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

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

	Entity* CreateEntity( const String& name, const String& filePath );

protected:
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:

	/// Root scene node
	SceneNode* mSceneRoot;

	/// Keep track of all scene node
    vector<SceneNode*> mAllSceneNodes;

	SceneNode* mSkyBoxNode;
	
};


}


#endif // SceneManager_h__

