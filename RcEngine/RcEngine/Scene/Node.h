#ifndef Node_h__
#define Node_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>

namespace RcEngine {

/**
 * Class representing a general-purpose node, has transform information, which is 
 * stored in local coordinated system relative to it's parent.
 * This is an abstract class - concrete classes are based on this for specific 
 * purposes, e.g. SceneNode, Bone
 */
class _ApiExport Node
{
public:
	Node();
	Node( const String& name );
	virtual ~Node();

	/**
	 * Set node name.
	 */
	void SetName( const String& name )	{ mName = name; }

	/**
	 * Get node name.
	 */
	const String& GetName() const { return mName; }
	
	/**
	 * Set node local position relative to its parent.
	 */
	void SetPosition(const Vector3f& position);

	/** 
	 * Get node local position relative to its parent.
	 */
	const Vector3f& GetPosition() const { return mPosition; }

	/**
	 * Set node local rotation relative to its parent.
	 */
	void SetRotation(const Quaternionf& rotation);

	/**
	 * Get node local rotation relative to its parent.
	 */
	const Quaternionf& GetRotation() const	{ return mRotation; }

	/**
	 * Set node local scale relative to its parent.
	 */
	void SetScale(const Vector3f& scale);

	/**
	 * Get node local scale relative to its parent.
	 */
	const Vector3f& GetScale() const { return mScale; }

	/**
	 * Set node local direction relative to its parent.
	 */
	void SetDirection( const Vector3f& direction );

	/**
	 * Set node local transform relative to its parent.
	 */
	void SetTransform( const Vector3f& position, const Quaternionf& rotation );

	/**
	 * Set node local transform relative to its parent.
	 */
	void SetTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale );

	/**
	 * Get local transform matrix relative to its parent.
	 */
	Matrix4f GetTransform() const; 

	/** 
	 * Set node world position.
	 */
	void SetWorldPosition( const Vector3f& position );

	/**
	 * Get world position.
	 */
	Vector3f GetWorldPosition() const;

	/**
	 * Set world rotation.
	 */
	void SetWorldRotation( const Quaternionf& rotation );

	/**
	 * Get world rotation.
	 */
	Quaternionf GetWorldRotation() const;

	/**
	 * Set world direction.
	 */
	void SetWorldDirection( const Vector3f& direction );

	/**
	 * Get world direction.
	 */
	Vector3f GetWorldDirection() const;

	/**
	 * Set world scale.
	 */
	void SetWorldScale( const Vector3f& scale );

	/**
	 * Get world scale.
	 */
	Vector3f GetWorldScale() const;

	/**
	 * Set world transform matrix.
	 */
	void SetWorldTransform( const Vector3f& position, const Quaternionf& rotation );

	/**
	 * Set world transform matrix.
	 */
	void SetWorldTransform( const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale );

	/**
	 * Get world transform matrix.
	 */
	const Matrix4f& GetWorldTransform() const;

	/** 
	 * Get attached child node count.
	 */
	uint32_t GetNumChildren( bool recursive = false ) const;

	/**
	 * Get a child node with specify name.
	 */

	Node* GetChild( const String& name ) const;

	/**
	 * Creates an named new Node as a child of this node.
	 */
	Node* CreateChild( const String& name, const Vector3f& translate,  const Quaternionf& rotate );

	/**
	 * Add a new child node.
	 */
	void AttachChild( Node* child );

	/**
	 * Remove a new child node.
	 * Does not delete the node, just detaches it from this parent, potentially to be reattached elsewhere. 
	 */
	void DetachChild( Node* child );

	/**
	 * Detach all child node.
	 */
	void DetachAllChildren();

	/**
	 * Set parent node.
	 */
	void SetParent( Node* parent );

	/**
	 * Get parent node.
	 */
	Node* GetParent() const { return mParent; }

	bool IsDirty() const { return mDirty; }
	
	void MarkDirty();

	void Update( float tick );

protected:

	/**
	 * Do derived class specify creation, derived class must overload this method.
	 */
	virtual Node* CreateChildImpl( const String& name ) = 0;

	virtual void OnUpdate( float tick ) = 0;

	virtual void OnChildNodeAdded( Node* node ) ;
	virtual void OnChildNodeRemoved( Node* node );

	void UpdateWorldTransform() const;

protected:

	Node* mParent;

	vector<Node*> mChildren;

	String mName;

	Vector3f mPosition;

	Quaternionf mRotation;

	Vector3f mScale;

	mutable Matrix4f mWorldTransform;

	mutable bool mDirty;

	/// Flag indicating that all children need to be updated
	mutable bool mWorldTransformDirty;

	mutable bool mWorldBoundDirty;
};

}



#endif // Node_h__
