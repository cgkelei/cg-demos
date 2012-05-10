#pragma once
#ifndef _MathUtil__H
#define _MathUtil__H

//  [8/23/2011 hustruan]
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
//#include "Quaternion.h"

namespace RcEngine{
namespace Math {

//////////////////////////////////////////////////////////////////////////
// 创建各种变化矩阵
//	
//
//////////////////////////////////////////////////////////////////////////

namespace MathUtil
{
	float const PI		= 3.141592f;			// PI

	template< typename Real, int32_t Size >
	inline typename  Real
	SquaredLength(const Vector<Real, Size>& vec);

	template< typename Real, int32_t Size >
	inline typename Real
	Length(const Vector<Real, Size>& vec);
		
	template< typename Real, int32_t Size >
	inline  typename Real
	Dot( const Vector<Real, Size>& lhs, const Vector<Real, Size>& rhs );

	template< typename Real, int32_t Size >
	inline  Vector<Real, Size>
	Normalize(const Vector<Real, Size>& vec);

	// Plane
	//////////////////////////////////////////////////////////////////////////
	template< typename Real >
	inline Plane3<Real>
	Normalize(const Plane3<Real>& plane);

	
	// 3D 向量
	//////////////////////////////////////////////////////////////////////////
	template<typename Real>
	inline  Vector<Real, 3> 
	Cross(const Vector<Real, 3>& lhs, const Vector<Real, 3>& rhs);
		

	//////////////////////////////////////////////////////////////////////////
	template<typename Real>
	inline Matrix4<Real> 
	CreateLookAtMatrixLH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp);
		
	template<typename Real>
	inline Matrix4<Real> 
	CreateLookAtMatrixRH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp);
		
	template<typename Real>
	inline Matrix4<Real>
	CreatePerspectiveFovLH(Real fovy, Real aspect,  Real zNear,  Real zFar);
		
	template<typename Real>
	inline Matrix4<Real> 
	CreatePerspectiveFovRH(Real fovy, Real aspect, Real zNear,  Real zFar);
		
	template<typename Real>
	inline Matrix4<Real> 
	CreatePerspectiveLH(Real width, Real height, Real zNear,  Real zFar);
		
	template<typename Real>
	inline Matrix4<Real> 
	CreatePerspectiveRH(Real width, Real height, Real zNear,  Real zFar);
		
	template<typename Real>
	inline Matrix4<Real> CreateRotationX(Real angle);
		
	template<typename Real>
	inline Matrix4<Real> CreateRotationY(Real angle);
		
	template<typename Real>
	inline Matrix4<Real> CreateRotationZ(Real angle);

	template<typename Real>
	inline Matrix4<Real> CreateRotationAxis(const Vector<Real,3>& axis, Real angle);

	template<typename Real>
	inline Matrix4<Real> CreateScaling(Real sX, Real sY, Real sZ);
		
	template<typename Real>
	inline Matrix4<Real> CreateTranslation(const Vector<Real,3>& trans);
		
	template<typename Real>
	inline Matrix4<Real> CreateTranslation(Real x, Real y, Real z);





	////Quaternion
	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionNormalize(const Quaternion<Real>& quat);

	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionConjugate(const Quaternion<Real>& quat);


	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionMultiply(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2);

	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionFromRotationMatrix(const Matrix4<Real>& rotMat);

	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionFromRotationAxis(const Vector<Real, 3>& axis, Real angleRadius);
	//
	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionFromRotationYawPitchRoll(Real yaw, Real pitch, Real roll);

	//template <typename Real>
	//inline Quaternion<Real> 
	//QuaternionSlerp(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2, Real t);

	//template <typename Real>
	//inline void
	//QuaternionToAxisAngle(const Quaternion<Real>& quat, Vector<Real, 3>& axis, Real& angle);


	#include "MathUtil.inl"

}

} // Namespace Math
} // Namespace RcEngine








#endif