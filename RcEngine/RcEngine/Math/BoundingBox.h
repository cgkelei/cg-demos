#ifndef BoundingBox_h__
#define BoundingBox_h__

#include <Math/Math.h>
#include <Math/Vector.h>

namespace RcEngine {

template <typename Real>
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
	 * Reset the bounding box to undefined.
	 */
	void SetNull()	{ Defined = false; }

	/**
	 * Return center of box.
	 */
	Vector<Real,3> Center() const { return (Min + Max) * Real(0.5); }

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
	 * Determines whether contains the specified point.
	 */
	ContainmentType Contains( const Vector<Real,3>& point );

	/**
	 * Determines whether a sphere intersects the specified object.
	 */
	bool Intersects( const BoundingBox<Real>& box );


public:
	Vector<Real,3> Max;
	Vector<Real,3> Min;
	bool Defined;
};

typedef BoundingBox<float> BoundingBoxf;
typedef BoundingBox<double> BoundingBoxd;

/**
 * Return a new bounding box merged  from two given bounding box 
 */
template<typename Real>
BoundingBox<Real> Merge( const BoundingBox<Real>& box1, const BoundingBox<Real>& box2 );

#include "Math/BoundingBox.inl"


} // Namespace RcEngine


#endif // BoundingBox_h__
