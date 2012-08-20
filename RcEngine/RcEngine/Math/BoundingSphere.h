#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine {


template<typename Real>
class BoundingSphere
{
public:
	BoundingSphere(): Defined(false) { }

	BoundingSphere(const Vector<Real, 3>& center, Real radius)
		: Center(center), Radius(radius), Defined(true) {}

	BoundingSphere(const BoundingSphere<Real>& rhs)
		: Center(rhs.Center), Radius(rhs.Radius), Defined(true) {}

	BoundingSphere& operator = (const BoundingSphere<Real>& rhs)
	{
		if (this != &rhs)
		{
			Center = rhs.Center;
			Radius = rhs.Radius;
			Defined = rhs.Defined;
		}
		return *this;
	}

	bool operator == ( const BoundingSphere<Real>& rhs)
	{
		return (Center == rhs.Center) && (Radius == rhs.Radius);
	}

	/**
	 * Merge a point
	 */
	void Merge( const Vector<Real, 3>& point );
	
	/**
	 * Merge a sphere
	 */
	void Merge( const BoundingSphere<Real>& sphere );

	/**
	 * Determines whether contains the specified sphere.
	 */
	ContainmentType Contains( const BoundingSphere<Real>& sphere );

	/**
	 * Determines whether contains the specified point.
	 */
	ContainmentType Contains( const Vector<Real,3>& point );

	/**
	 * Determines whether a sphere intersects the specified object.
	 */
	bool Intersects( const BoundingSphere<Real>& sphere );


public:
	Vector<Real, 3> Center;
	Real Radius;

private:
	bool Defined;
};

typedef BoundingSphere<float> BoundingSpheref;
typedef BoundingSphere<double> BoundingSphered;

/**
 * Return a new bounding sphere merged  from two given bounding sphere 
 */
template<typename Real>
BoundingSphere<Real> Merge(const BoundingSphere<Real>& sphere1,const BoundingSphere<Real>& sphere2 );

#include "Math/BoundingSphere.inl"


} // Namespace RcEngine

#endif // BoundingSphere_h__
