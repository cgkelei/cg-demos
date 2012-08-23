#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>

namespace RcEngine {

class Skeleton;

class _ApiExport Joint : public Node
{
public:
	Joint(Skeleton* skeleton, const String& name);


	void SetSkeleton( Skeleton* skeleton );

	Skeleton* GetSkeleton()	const { return mSkeleton; }

protected:
	Node* CreateChildImpl( const String& name );
	void OnUpdate( float tick );

	Skeleton* mSkeleton;

	/// Reset position.
	Vector3f InitialPosition;
	/// Reset rotation.
	Quaternionf InitialRotation;
	/// Reset scale.
	Vector3f InitialScale;
	/// Offset matrix.
	Matrix4f OffsetMatrix;
};

class _ApiExport Skeleton
{
public:
	Skeleton();
	~Skeleton();

	vector<Joint>& GetJoints() { return mJoints; }

private:
	vector<Joint> mJoints;
};

} // Namespace RcEngine

#endif // Skeleton_h__