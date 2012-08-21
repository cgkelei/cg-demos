#ifndef Scene_h__
#define Scene_h__

//  [8/20/2012 Ruan]

#include <Core/Prerequisites.h>

namespace RcEngine {


class _ApiExport Scene
{
public:
	Scene();
	virtual ~Scene();


	/**
	 * Add the specify node to scene
	 */
	void AddSceneNode( SceneNode* node );

	void RemoveSceneNode( SceneNode* node );


protected:

	/// Root scene node
	SceneNode* mSceneRoot;

	unordered_map<String, SceneNode*> mAllSceneNodes;

	SceneNode* mSkyBoxNode;
	
};


}




#endif // Scene_h__
