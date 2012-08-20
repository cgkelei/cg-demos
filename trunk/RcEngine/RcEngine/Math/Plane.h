#pragma once
#ifndef _Plane__H
#define _Plane__H

//  [8/24/2011 hustruan]

#include <Math/MathUtil.h>

namespace RcEngine{


// The plane is represented as Dot(N,X) = c where N is a unit-length
// normal vector, c is the plane constant, and X is any point on the
// plane.  The user must ensure that the normal vector satisfies this
// condition.
	
enum PlaneSide
{
	No_Side,
	Positive_Side,
	Negative_Side,
	Both_Side
};

template< typename Real >
class Plane3
{

public:
	Plane3();
	Plane3(const Plane3& rhs);

	// specify N and c directly
	Plane3(const Vector<Real, 3>& rkNormal, Real fConstant);

	// N is specified, C = Dot(N,P) where P is on the plane
	Plane3(const Vector<Real, 3>& rkNormal, const Vector<Real, 3>& rkP);
		
	// N = Cross(P1-P0,P2-P0)/Length(Cross(P1-P0,P2-P0)), C = Dot(N,P0) where
	// P0, P1, P2 are points on the plane.
	Plane3(const Vector<Real, 3>& rkPoint1, const Vector<Real, 3>& rkPoint2,
		const Vector<Real, 3>& rkPoint3);

	// assignment
	Plane3& operator= (const Plane3& rkPlane);

	PlaneSide WhichSide (const Vector<Real, 3>& rkPoint) const;
	Real DistanceTo(const Vector<Real, 3>& rkQ) const;
		
	Vector<Real, 3> Normal;
	Real Constant;
};

#include <Math/Plane.inl>

typedef Plane3<float> Plane3f;


} // Namespace RcEngine



#endif