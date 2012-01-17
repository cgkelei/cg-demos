#ifndef _Matrix__H
#define _Matrix__H

#include "Vector.h"

namespace RcEngine{
namespace Math {


#define MatrixItem(iRow, iCol) ( iRow*4 + iCol )

template<typename Real>
class Matrix4
{
public:
	Matrix4();
	Matrix4(const Real* rhs);
	Matrix4(const Matrix4& rhs);
		
	template<typename T>
	Matrix4(const Matrix4<T>& rhs);

	Matrix4(Real f11, Real f12, Real f13, Real f14,
		Real f21, Real f22, Real f23, Real f24,
		Real f31, Real f32, Real f33, Real f34,
		Real f41, Real f42, Real f43, Real f44);

	void MakeIdentity();
	void MakeZero();

	// member access
	inline const Real* operator() () const;
	inline Real* operator() () ;
	inline const Real* operator[] (int iRow) const;
	inline Real* operator[] (int iRow);
	inline Real operator() (int iRow, int iCol) const;
	inline Real& operator() (int iRow, int iCol);
	void SetRow (int iRow, const Vector<Real, 4>& rhs);
	Vector<Real, 4> GetRow (int iRow) const;
	void SetColumn (int iCol, const Vector<Real, 4>& rhs);
	Vector<Real, 4> GetColumn (int iCol) const;
		
	// assignment
	inline Matrix4& operator= (const Matrix4& rhs);

	// comparison
	bool operator== (const Matrix4& rhs) const;
	bool operator!= (const Matrix4& rhs) const;
		
	// arithmetic operations
	inline Matrix4 operator+ (const Matrix4& rhs) const;
	inline Matrix4 operator- (const Matrix4& rhs) const;
	inline Matrix4 operator* (const Matrix4& rhs) const;
	inline Matrix4 operator* (Real fScalar) const;
	inline Matrix4 operator/ (Real fScalar) const;
	inline Matrix4 operator- () const;

	// arithmetic updates
	inline Matrix4& operator+= (const Matrix4& rhs);
	inline Matrix4& operator-= (const Matrix4& rhs);
	inline Matrix4& operator*= (Real fScalar);
	inline Matrix4& operator/= (Real fScalar);

	// matrix times vector
	inline Vector<Real, 4> operator* (const Vector<Real, 4>& rhs) const;  // M * V
		
	Matrix4 GetTransposed() const;
	void Transpose();
	Real Determinant () const;
	
public:

	union
	{
		struct{ Real Elements[16]; };
		struct
		{
			Real M11, M12, M13, M14;
			Real M21, M22, M23, M24;
			Real M31, M32, M33, M34;
			Real M41, M42, M43, M44;
		};
	};

private:
	// support for comparisons
	int CompareArrays (const Matrix4& rhs) const;
		
};
	
// c * M
template <typename Real>
inline Matrix4<Real> operator* (Real fScalar, const Matrix4<Real>& rhs);

// v^T * M
template <typename Real>
inline Vector<Real, 4> operator* (const Vector<Real, 4>& lhs, const Matrix4<Real>& rhs);

	
#include "Matrix.inl"

typedef Matrix4<float> Matrix4f;
typedef Matrix4<double> Matrix4d;

} // Namespace Math



} // Namespace RcEngine

#endif // _Matrix__H