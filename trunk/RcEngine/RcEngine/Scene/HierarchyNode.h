#ifndef HierarchyNode_h__
#define HierarchyNode_h__

#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>

namespace RcEngine {


template <typename NodeType>
class HierarchyNode
{
public:
	HierarchyNode();
	virtual ~HierarchyNode();

	const Quaternionf& GetRotation() const { return mRotation; }
	const Vector3f& GetScale() const	{ return mScale; }
	const Vector3f& GetTranslation() const	{ return mTranslation; }

	const Quaternionf& GetWorldRotation() const;
	const Vector3f& GetWorldScale() const;
	const Vector3f& GetWorldTranslation() const;

	NodeType* GetChild( const String& name );
	NodeType* CreateChild();
	void AttachChild( NodeType* node );
	void DetachChild( NodeType* node );

protected:
	void PropagateDirtyDown( uint32_t dirtyFlag );
	void PropagateDirtyUp( uint32_t dirtyFlag );

	virtual void OnAttach() {}
	virtual void OnDetach() {}


protected:
	
	String mName;

	Vector3f mScale;
	Vector3f mTranslation;
	Quaternionf mRotation;

	// cached world 
	mutable Vector3f mWorldScale;
	mutable Vector3f mWorldTranslation;
	mutable Quaternionf mWorldRotation;

	mutable Matrix4f mWorldTransform;

	NodeType* mParent;
	std::vector<NodeType*> mChildrend;

};

}


#endif // HierarchyNode_h__