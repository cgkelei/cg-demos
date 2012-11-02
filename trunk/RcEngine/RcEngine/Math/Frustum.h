#ifndef Frustum_h__
#define Frustum_h__

#include <Math/Plane.h>
#include <Math/Matrix.h>
#include <Math/BoundingSphere.h>
#include <Math/BoundingBox.h>

namespace RcEngine {

enum FrustumPlaneSide
{
	FPS_Left = 0,
	FPS_Right = 1,
	FPS_Top = 2,
	FPS_Bottom = 3,
	FPS_Near = 4,
	FPS_Far = 5,
};

template<typename Real>
class Frustum
{
public:
	typedef Real value_type;

public:
	Frustum();

	/**
	 * Update the view frustum planes using the current view/projection matrices.
	 */
	void Update(const Matrix4<Real>& viewProjection);

	/**
	 * Determines whether the frustum contain specify sphere.
	 */
	ContainmentType Contain( const BoundingSphere<Real>& sphere ); 

	ContainmentType Contain( const BoundingBox<Real>& box ); 

public:
	Plane<Real> Planes[6];
	Vector<Real,3>  mOrigin;
	Vector<Real,3>  mCorner[8];  // Corner points
};

typedef Frustum<float> Frustumf;

#include <Math/Frustum.inl>

}


#endif // Frustum_h__