#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include <Math/Vector.h>
#include <Math/BoundingBox.h>

namespace RcEngine {
namespace Math {

enum ContainmentType 
{
	CT_Disjoint,
	CT_Contains,
	CT_Intersects
};

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
	 * Determines whether contains the specified box.
	 */
	ContainmentType Contains( const BoundingBox<Real>& box );

	/**
	 * Determines whether contains the specified point.
	 */
	ContainmentType Contains( const Vector<Real,3>& point );

	/**
	 * Determines whether a sphere intersects the specified object.
	 */
	bool Intersects( const BoundingBox<Real>& box );

	/**
	 * Determines whether a sphere intersects the specified object.
	 */
	bool Intersects( const BoundingSphere<Real>& sphere );


public:
	Vector<Real, 3> Center;
	Real Radius;
	bool Defined;
};

typedef BoundingSphere<float> BoundingSpheref;
typedef BoundingSphere<double> BoundingSphered;

/**
 * Create a new bounding sphere form a bounding box
 */
template<typename Real>
BoundingSphere<Real> FromBox( const BoundingBox<Real>& box );

/**
 * Return a new bounding sphere merged  from two given bounding sphere 
 */
template<typename Real>
BoundingSphere<Real> Merge(const BoundingSphere<Real>& sphere1,const BoundingSphere<Real>& sphere2 );

#include "Math/BoundingSphere.inl"

} // Namespace Math
} // Namespace RcEngine

#endif // BoundingSphere_h__
