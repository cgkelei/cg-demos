#ifndef SceneNode_h__
#define SceneNode_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>
#include <Math/BoundingSphere.h>

namespace RcEngine {

class SceneObject;
class SceneNodeVisitor;

class _ApiExport SceneNode : public Node
{
public:
	SceneNode(SceneManager* scene, const String& name);
	virtual ~SceneNode();

	const BoundingSpheref& GetWorldBoundingShpere() const;

	uint32_t GetNumAttachedObjects() const { return mAttachedObjects.size(); }

	void AttachObject( SceneObject* obj );

	void DetachOject( SceneObject* obj );

	void DetachAllObject(); 

	/**
	 * Get an attached object by given name.
	 */
	SceneObject* GetAttachedObject( const String& name );

	/**
	 * Creates an named new SceneNode as a child of this node.
	 */
	SceneNode* CreateChildSceneNode( const String& name, const Vector3f& translate = Vector3f::Zero(),  const Quaternionf& rotate = Quaternionf::Identity());

	/** 
	 * Return the scene to which a node belongs.
	 */
	SceneManager* GetScene() const { return mScene; }

	/**
	 * Sets the scene to which a node belongs.
	 */
	void SetScene( SceneManager* scene );


	void FindVisibleObjects( Camera* cam );

	
protected:
	virtual Node* CreateChildImpl( const String& name );

	/**
	 * Update world bound, this will merge a bound including all attached object 
	 * and the bound of all child scene node.
	 */
	void UpdateWorldBounds() const;

	void OnPostUpdate( );

protected:

	mutable BoundingSpheref mWorldBounds;



	Matrix4f mWorldTransform

	SceneManager* mScene;


	std::vector<SceneObject*> mAttachedObjects;
};


}




#endif // SceneNode_h__
