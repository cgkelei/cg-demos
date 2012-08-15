#pragma once
#ifndef _Vector__H
#define _Vector__H

#include <cstdint>
#include <iostream>

//  [8/22/2011 hustruan]
namespace RcEngine {
namespace Math {

template< typename Real, int32_t Size >
class Vector
{
public:
	Vector();

	Vector(Real fX, Real fY);
	Vector(Real fX, Real fY, Real fZ);
	Vector(Real fX, Real fY, Real fZ, Real fW);

	Vector(const Real* fTuple);
	Vector(Real value);
		
	template<typename T>
	Vector(const Vector<T, Size>& rhs);

	// assignment
	inline Vector& operator= (const Vector& rhs);

	// coordinate access
	inline const Real* operator() (void)const;
	inline Real* operator() (void) ;
	inline Real operator[] (size_t i) const;
	inline Real& operator[] (size_t i);
	inline Real X () const;
	inline Real& X ();
	inline Real Y () const;
	inline Real& Y ();
	inline Real Z () const;
	inline Real& Z ();
	inline Real W () const;
	inline Real& W ();

	// arithmetic operations
	inline Vector operator+ (const Vector<Real, Size>& rhs) const;
	inline Vector operator- (const Vector<Real, Size>& rhs) const;
	inline Real   operator* (const Vector<Real, Size>& rhs) const;
	inline Vector operator* (Real fScalar) const;
	inline Vector operator/ (Real fScalar) const;
	inline Vector operator- () const;

	friend Vector operator* (Real fScalar, const Vector<Real, Size>& rhs)
	{
		Vector<Real, Size> result;
		for(int32_t i = 0; i < Size; i++)
			result[i] =  fScalar * rhs[i];

		return result;
	}

	// arithmetic updates
	inline Vector& operator+= (const Vector& rhs);
	inline Vector& operator-= (const Vector& rhs);
	inline Vector& operator*= (Real fScalar);
	inline Vector& operator/= (Real fScalar);



	// comparison
	bool operator== (const Vector& rhs) const;
	bool operator!= (const Vector& rhs) const;

	// vector operations
	inline Real Length () const;
	inline Real SquaredLength () const;
	inline Real Dot(const Vector& rhs) const;
	inline void Normalize ();

	inline static Vector Zero();

private:
	int32_t CompareArrays (const Vector& rhs) const;
	Real mTuple[Size];
};

// debugging output
template <class Real, int32_t Size>
std::ostream& operator<< (std::ostream& rkOStr, const Vector<Real, Size>& rhs);


#include <Math/Vector.inl>

typedef Vector<float, 2> Vector2f;
typedef Vector<double, 2> Vector2d;
typedef Vector<float,3> Vector3f;
typedef Vector<float,4> Vector4f;
typedef Vector<double,3> Vector3d;
typedef Vector<double,4> Vector4d;

} // Namespace Math
} // Namespace RcEngine
#endif