#ifndef BoundingSphere_h__
#define BoundingSphere_h__

#include <Math/Vector.h>

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
	BoundingSphere(), Defined(false) { }

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
		}
		return *this;
	}

	bool operator == ( const BoundingSphere<Real>& rhs)
	{
		return (Center == rhs.Center) && (Radius == rhs.Radius);
	}

	void Merge( const Vector<Real, 3>& point );
	void Merge( const BoundingSphere<Real>& sphere );


	/**
	 * Determines whether contains the specified sphere.
	 */
	ContainmentType Contains( const BoundingSphere<Real>& sphere );

	/**
	 * Determines whether contains the specified point.
	 */
	ContainmentType Contains( const Vector<Real,3>& vector );

	bool Intersects( const BoundingBox<Real>& box );

	bool Intersects( const BoundingSphere<Real>& sphere );


public:
	Vector<Real, 3> Center;
	Real Radius;
	bool Defined;
};




typedef BoundingSphere<float> BoundingSpheref;
typedef BoundingSphere<double> BoundingSphered;

template<typename Real>
class BoundingBox
{
public:
	BoundingBox();

	BoundingBox(const Vector<Real,3>& min, const Vector<Real,3>& max)
		: Min(min), Max(max){ }

	BoundingBox(const BoundingBox<Real>& rhs)
		: Min(rhs.Min), Max(rhs.Max) { }

	BoundingBox& operator = (const BoundingBox<Real>& rhs)
	{
		if (this != &rhs)
		{
			Max = rhs.Max;
			Min = rhs.Min;
		}
		return *this;
	}

	bool operator == ( const BoundingBox<Real>& rhs)
	{
		return (Min == rhs.Min) && (Max == rhs.Max);
	}

	void Merge( const Vector<Real,3>& point );
	void Merge( const BoundingBox<Real>& box );


	ContainmentType Contains( const BoundingBox<Real>& box2 );

	ContainmentType Contains( const BoundingSphere<Real>& sphere );

	ContainmentType Contains( const Vector<Real,3>& vector );

	bool Intersects( const BoundingBox<Real>& box );

	bool Intersects( const BoundingSphere<Real>& sphere );


public:
	Vector<Real,3> Max;
	Vector<Real,3> Min;
	bool Defined;
};

template<typename Real>
void RcEngine::Math::BoundingBox<Real>::Merge( const BoundingBox<Real>& box )
{

}

template<typename Real>
void RcEngine::Math::BoundingBox<Real>::Merge( const Vector<Real,3>& point )
{

}

typedef BoundingBox<float> BoundingBoxf;
typedef BoundingBox<double> BoundingBoxd;


#include "Math/BoundingSphere.inl"

} // Namespace Math
} // Namespace RcEngine

#endif // BoundingSphere_h__
