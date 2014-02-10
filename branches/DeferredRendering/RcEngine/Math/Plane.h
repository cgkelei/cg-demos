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
 * Plane: N*P+D = 0
 */
template< typename Real >
class Plane
{
public:
	typedef Real value_type;

public:
	Plane();
	Plane(const Vector<Real, 3>& nroaml, Real distance);
	Plane(const Vector<Real, 3>& nroaml, const Vector<Real, 3>& point);	
	Plane(const Vector<Real, 3>& point1, const Vector<Real, 3>& point2, const Vector<Real, 3>& point3);
	Plane(const Plane& rhs);

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