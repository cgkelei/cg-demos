#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>
#include <Math/Quaternion.h>

namespace RcEngine {
namespace Render {

using namespace RcEngine::Math;

struct _ApiExport Joint
{
	Joint();

		/// Bone name.
	String Name;
	/// Parent bone index.
	uint32_t ParentIndex;
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

} // Namespace Render
} // Namespace RcEngine

#endif // Skeleton_h__