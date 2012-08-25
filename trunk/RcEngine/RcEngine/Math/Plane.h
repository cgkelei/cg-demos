#pragma once
#ifndef _Plane__H
#define _Plane__H

//  [8/24/2011 hustruan]
#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine{

enum PlaneSide
{
	No_Side,
	Positive_Side,
	Negative_Side,
	Both_Side
};

/** 
 * The plane is represented as Dot(N,X) - Distance = 0 where N is a unit-length
 * normal vector, Distance is the distance of the plane along its normal from the
 * origin, and X is any point on the plane. 
 */
template< typename Real >
class Plane
{
public:
	Plane();
	Plane(const Plane& rhs);

	// specify N and c directly
	Plane(const Vector<Real, 3>& nroaml, Real distance);

	// N is specified, C = Dot(N,P) where P is on the plane
	Plane(const Vector<Real, 3>& nroaml, const Vector<Real, 3>& point);
		
	// N = Cross(P1-P0,P2-P0)/Length(Cross(P1-P0,P2-P0)), D = -Dot(N,P0) where
	// P0, P1, P2 are points on the plane.
	Plane(const Vector<Real, 3>& point1, const Vector<Real, 3>& point2, const Vector<Real, 3>& point3);

	// assignment
	Plane& operator= (const Plane& rhs);

	PlaneSide WhichSide (const Vector<Real, 3>& point) const;

	Real DistanceTo(const Vector<Real, 3>& point) const;

	void Normalize(Real epsilon = Math<Real>::ZERO_TOLERANCE);
		
public:
	Vector<Real, 3> Normal;
	Real Distance;
};

#include <Math/Plane.inl>

typedef Plane<float> Planef;


} // Namespace RcEngine



#endif