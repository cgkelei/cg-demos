#ifndef SceneNode_h__
#define SceneNode_h__


#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>
#include <Math/BoundingSphere.h>
#include <Math/BoundingBox.h>

namespace RcEngine {


class SceneNodeVisitor;


class _ApiExport SceneNode
{
public:
	SceneNode();
	virtual ~SceneNode();

	void SetName( const String& name )	{ mName = name; }

	const BoundingSpheref& GetBoundingSphere() const { return mBoundingSphere; }

	void SetPosition(const Vector3f& position);

	void SetRotation(const Quaternionf& rotation);

	void SetDirection(const Vector3f& direction);

	void SetScale(const Vector3f& scale);

	void SetTransform(const Vector3f& position, const Quaternionf& rotation);

	void SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale);

	void SetWorldPosition(const Vector3f& position);

	void SetWorldRotation(const Quaternionf& rotation);

	void SetWorldDirection(const Vector3f& direction);

	void SetWorldScale(const Vector3f& scale);

	void SetWorldTransform(const Vector3f& position, const Quaternionf& rotation);

	void SetWorldTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale);

	/**
	 * Get local transform matrix relative to its parent 
	 */
	Matrix4f GetLocalTransform() const;

	/**
	 * Get world position.
	 */
	Vector3f GetWorldPosition() const;

	/**
	 * Get world rotation.
	 */
	Quaternionf GetWorldRotation() const;
	
	/**
	 * Get world direction.
	 */
	Vector3f GetWorldDirection() const;

	/**
	 * Get world scale.
	 */
	Vector3f GetWorldScale() const;

	/**
	 * Get world transform matrix.
	 */
	const Matrix4f& GetWorldTransform() const;

	/** 
	 * Get attached child node count.
	 */
	uint32_t GetNumChildren( bool recursive = false ) const;

	/**
	 * Attach a new child node.
	 */
	virtual void AttachChild( SceneNode* child );

	/**
	 * Detach a new child node.
	 */
	virtual void DetachChild( SceneNode* child );

	/**
	 * Detach all child node.
	 */
	void RemoveAllChildren();

	void SetParent( SceneNode* parent );

	SceneNode* GetParent() const { return mParent; }

	Scene* GetScene() const	{ return mScene; }

	/**
	 * Creates an unnamed new SceneNode as a child of this node.
	 */
	SceneNode* CreateChildSceneNode( const Vector3f& translate,  const Quaternionf& rotate );

	/**
	 * Creates an named new SceneNode as a child of this node.
	 */
	SceneNode* CreateChildSceneNode( const String& name, const Vector3f& translate,  const Quaternionf& rotate );

	bool IsDirty() const { return mDirty; }
	void MarkDirty();



private:

	void UpdateWorldTransform() const;
	void UpdateWorldBound() const;


protected:

	Scene* mScene;

	SceneNode* mParent;

	std::list<SceneNode*> mChildren;

	String mName;

	Vector3f mPosition;

	Quaternionf mRotation;

	Vector3f mScale;

	mutable BoundingSpheref mBoundingSphere;

	mutable Matrix4f mWorldTransform;

	mutable bool mDirty;
};

}




#endif // SceneNode_h__
