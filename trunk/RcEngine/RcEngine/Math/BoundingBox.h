#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <Math/Vector.h>
#include <Math/BoundingSphere.h>

namespace RcEngine {
namespace Math {

template<typename Real>
class BoundingBox
{
public:
	BoundingBox() : Defined(false) { }

	BoundingBox(const Vector<Real,3>& min, const Vector<Real,3>& max)
		: Min(min), Max(max), Defined(true) { }

	BoundingBox(const BoundingBox<Real>& rhs)
		: Min(rhs.Min), Max(rhs.Max), Defined(rhs.Defined) { }

	BoundingBox& operator = (const BoundingBox<Real>& rhs)
	{
		if (this != &rhs)
		{
			Max = rhs.Max;
			Min = rhs.Min;
			Defined = rhs.Defined;
		}
		return *this;
	}

	bool operator == ( const BoundingBox<Real>& rhs)
	{
		return (Min == rhs.Min) && (Max == rhs.Max);
	}

	/**
	 * Merge a point
	 */
	void Merge( const Vector<Real,3>& point );
	
	/**
	 * Merge another box
	 */
	void Merge( const BoundingBox<Real>& box );

	/**
	 * Determines whether contains the specified box.
	 */
	ContainmentType Contains( const BoundingBox<Real>& box );

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
	bool Intersects( const BoundingBox<Real>& box );

	/**
	 * Determines whether a sphere intersects the specified object.
	 */
	bool Intersects( const BoundingSphere<Real>& sphere );

public:
	Vector<Real,3> Max;
	Vector<Real,3> Min;
	bool Defined;
};

typedef BoundingBox<float> BoundingBoxf;
typedef BoundingBox<double> BoundingBoxd;

/**
 * Create a new bounding box form a bounding sphere
 */
template<typename Real>
BoundingBox<Real> FromSphere( const BoundingSphere<Real>& sphere );

/**
 * Return a new bounding box merged  from two given bounding box 
 */
template<typename Real>
BoundingBox<Real> Merge( const BoundingBox<Real>& box1, const BoundingBox<Real>& box2 );

#include "Math/BoundingBox.inl"

} // Namespace Math
} // Namespace RcEngine


#endif // BoundingBox_h__
