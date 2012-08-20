#ifndef SceneNode_h__
#define SceneNode_h__


#include <Core/Prerequisites.h>

namespace RcEngine {


class SceneNodeVisitor;


class _ApiExport SceneNode
{
public:
	SceneNode();
	virtual ~SceneNode();


protected:

	Scene* mScene;

	SceneNode* mParent;

	vector<SceneNode*> mChildren;
	

};

}




#endif // SceneNode_h__
