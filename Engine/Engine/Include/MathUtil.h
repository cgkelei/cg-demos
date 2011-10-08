#pragma once
#ifndef _MathUtil__H
#define _MathUtil__H

//  [8/23/2011 hustruan]
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"

namespace Engine
{
	//////////////////////////////////////////////////////////////////////////
	// 创建各种变化矩阵
	//	
	//
	//////////////////////////////////////////////////////////////////////////

	namespace MathUtil
	{
		template< typename Real, int Size >
		inline typename  Real
		SquaredLength(const Vector<Real, Size>& vec);

		template< typename Real, int Size >
		inline typename Real
		Length(const Vector<Real, Size>& vec);
		
		template< typename Real, int Size >
		inline  typename Real
		Dot( const Vector<Real, Size>& lhs, const Vector<Real, Size>& rhs );

		template< typename Real, int Size >
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
		
		template<typename Real>
		inline  void
		CreateLookAtMatrixLH(Matrix4<Real>& outMat,const Vector<Real,3>& vEye,
			const Vector<Real,3>& vAt, const Vector<Real,3>& vUp);
		
		template<typename Real>
		inline  void
		CreateLookAtMatrixRH(Matrix4<Real>& outMat,const Vector<Real,3>& vEye,
			const Vector<Real,3>& vAt, const Vector<Real,3>& vUp);
		
		template<typename Real>
		inline  void 
		CreatePerspectiveFovLH(Matrix4<Real>& out, Real fovy, Real aspect, 
			Real zNear,  Real zFar);
		
		template<typename Real>
		inline void 
		CreatePerspectiveFovRH(Matrix4<Real>& out, Real fovy, Real aspect, 
			Real zNear,  Real zFar);
		
		template<typename Real>
		inline  void
		CreatePerspectiveLH(Matrix4<Real>& out, Real width, Real height, 
			Real zNear,  Real zFar);
		
		template<typename Real>
		inline  void 
		CreatePerspectiveRH(Matrix4<Real>& out, Real width, Real height, 
			Real zNear,  Real zFar);
		
		template<typename Real>
		inline  void 
		CreateRotationX(Matrix4<Real>& out, Real angle);
		
		template<typename Real>
		inline  void 
		CreateRotationY(Matrix4<Real>& out, Real angle);
		
		template<typename Real>
		inline  void 
		CreateRotationZ(Matrix4<Real>& out, Real angle);
		
		// Builds a matrix that rotates around an arbitrary axis.
		template<typename Real>
		inline  void
		CreateRotationAxis(Matrix4<Real>& out,  const Vector<Real,3>& axis, Real angle);

		// Builds a matrix that scales along the x-axis, the y-axis, and the z-axis.
		template<typename Real>
		inline  void 
		CreateScaling(Matrix4<Real>& out, Real sX, Real sY, Real sZ);
		
		template<typename Real>
		inline  void 
		CreateTranslation(Matrix4<Real>& out, const Vector<Real,3>& trans);
		
		template<typename Real>
		inline  void 
		CreateTranslation(Matrix4<Real>& out, Real x, Real y, Real z);

		#include "MathUtil.inl"

	}



}




#endif