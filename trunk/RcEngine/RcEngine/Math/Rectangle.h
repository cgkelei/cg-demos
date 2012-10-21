#ifndef Rectangle_h__
#define Rectangle_h__

#include <Math/Math.h>

namespace RcEngine {

template<typename Real>
class Rectangle
{
public:
	Rectangle(Real x, Real y, Real width, Real height);
	Rectangle(const Rectangle& other);

	inline Real Left() const;
	inline Real Top() const;
	inline Real Right() const;
	inline Real Bottom() const;

	inline bool Contains(Real x, Real y) const;
	inline ContainmentType Contains(Real x, Real y, Real width, Real height) const;
	inline ContainmentType Contains(const Rectangle& other) const;

	inline bool Intersects(Real x, Real y, Real width, Real height) const;
	inline bool Intersects(const Rectangle& other) const;

public:
	Real X, Y;
	Real Width, Height;
};

///**
// * Creates a Rectangle defining the area where one rectangle overlaps with another rectangle.
// */
//template<typename Real>
//Rectangle<Real> Intersect(const Rectangle<Real>& lhs, const Rectangle<Real>& rhs);
//
///**
// * Creates a Rectangle defining the area which cover both rectangle.
// */
//template<typename Real>
//Rectangle<Real> Union(const Rectangle<Real>& lhs, const Rectangle<Real>& rhs);

typedef Rectangle<float> Rectanglef;
}



#endif // Rectangle_h__
