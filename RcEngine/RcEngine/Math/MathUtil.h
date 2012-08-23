#pragma once
#ifndef _MathUtil__H
#define _MathUtil__H

//  [8/23/2011 hustruan]
#include <Math/Math.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>
#include <Math/Plane.h>
#include <Math/Quaternion.h>
#include <Math/BoundingSphere.h>
#include <Math/BoundingBox.h>

namespace RcEngine{


const float PI		= 3.141592f;			// PI
const float RadianToDegree = 180.0f / PI;
const float DegreeToRadian = PI / 180.0f;

inline float ToDegree(float radian)	{ return radian * RadianToDegree;}
inline float ToRadian(float degree)	{ return degree * DegreeToRadian; }

/************************************************************************/
/* Vector                                                               */
/************************************************************************/
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

template<typename Real>
inline  Vector<Real, 3> 
Cross(const Vector<Real, 3>& lhs, const Vector<Real, 3>& rhs);

/************************************************************************/
/* Plane                                                                */
/************************************************************************/
template< typename Real >
inline Plane3<Real>
Normalize(const Plane3<Real>& plane);

	
/************************************************************************/
/* Matrix                                                               */
/************************************************************************/
template<typename Real>
inline Matrix4<Real> 
MatrixInverse(const Matrix4<Real>& mat);

/************************************************************************/
/* Transform Matrix                                                     */
/************************************************************************/
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
inline Matrix4<Real> 
CreateRotationX(Real angle);
		
template<typename Real>
inline Matrix4<Real> 
CreateRotationY(Real angle);
		
template<typename Real>
inline Matrix4<Real> 
CreateRotationZ(Real angle);

template<typename Real>
inline Matrix4<Real> 
CreateRotationAxis(const Vector<Real,3>& axis, Real angle);

template<typename Real>
inline Matrix4<Real> 
CreateRotationYawPitchRoll(Real yaw, Real pitch, Real roll);

template<typename Real>
inline Matrix4<Real> 
CreateScaling(Real sX, Real sY, Real sZ);
		
template<typename Real>
inline Matrix4<Real> 
CreateTranslation(const Vector<Real,3>& trans);
		
template<typename Real>
inline Matrix4<Real> 
CreateTranslation(Real x, Real y, Real z);


template<typename Real>
inline Matrix4<Real> 
CreateTransformMatrix( const Vector<Real, 3>& sacle, const Quaternion<Real>& rotation, const Vector<Real, 3>& translation );

template<typename Real>
inline void 
MatrixDecompose(Vector<Real, 3>& sacle, Quaternion<Real>& rotation, Vector<Real, 3>& translation, const Matrix4<Real>& mat);

template<typename Real>
inline Matrix4<Real>
RotationFromTransformMatrix( const Matrix4<Real>& transformMat);

template<typename Real>
inline Vector<Real,3>
TranslationFromTransformMatrix( const Matrix4<Real>& transformMat);

template<typename Real>
inline Vector<Real,3>
ScaleFromTransformMatrix( const Matrix4<Real>& transformMat);

/**
 * Get euler angle from rotation matrix
 */
template<typename Real>
inline void 
RotationMatrixToYawPitchRoll(Real& yaw, Real& pitch, Real& roll, const Matrix4<Real>& mat);

/**
 * Transforms vector (x, y, z, 1) by a given matrix.
 */
template<typename Real>
Vector<Real, 3> 
Transform(const Vector<Real, 3>& vec, const Matrix4<Real>& mat);

/**
 * Transforms vector (x, y, z, 1) by a given quaternion.
 */
template<typename Real>
Vector<Real, 3> 
Transform(const Vector<Real, 3>& vec, const Quaternion<Real>& quat);

/**
 * Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
 */
template<typename Real>
Vector<Real, 3> 
TransformCoord(const Vector<Real, 3>& vec, const Matrix4<Real>& mat);

template<typename Real>
BoundingBox<Real>
Transform( const BoundingBox<Real>& box, const Matrix4<Real>& matrix );

template<typename Real>
BoundingBox<Real>
TransformAffine( const BoundingBox<Real>& box, const Matrix4<Real>& matrix );

template<typename Real>
BoundingSphere<Real>
Transform( const BoundingSphere<Real>& sphere, const Matrix4<Real>& matrix );


/************************************************************************/
/* Quaternion                                                           */
/************************************************************************/
template <typename Real>
inline Real QuaternionLength(const Quaternion<Real>& quat);

template <typename Real>
inline Quaternion<Real> 
QuaternionNormalize(const Quaternion<Real>& quat);

template <typename Real>
inline Real QuaternionDot(const Quaternion<Real>& quat);

template <typename Real>
inline Quaternion<Real> 
QuaternionConjugate(const Quaternion<Real>& quat);

template <typename Real>
inline Quaternion<Real> 
QuaternionMultiply(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2);

template <typename Real>
Quaternion<Real> 
QuaternionInverse(const Quaternion<Real>& quat);

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationMatrix(const Matrix4<Real>& rotMat);

template <typename Real>
inline Matrix4<Real>
QuaternionToRotationMatrix(const Quaternion<Real>& rotQuat);

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationAxis(const Vector<Real, 3>& axis, Real angleRadius);

template <typename Real>
inline void
QuaternionToAxisAngle(const Quaternion<Real>& quat, Vector<Real, 3>& axis, Real& angle);

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationYawPitchRoll(Real yaw, Real pitch, Real roll);

template <typename Real>
inline void 
QuaternionToRotationYawPitchRoll(Real& yaw, Real& pitch, Real& roll, const Quaternion<Real>& quat);

template <typename Real>
inline Quaternion<Real> 
QuaternionSlerp(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2, Real t);

/**
 * Create a new bounding box form a bounding sphere
 */
template<typename Real>
BoundingBox<Real> FromSphere( const BoundingSphere<Real>& sphere );

/**
 * Create a new bounding sphere form a bounding box
 */
template<typename Real>
BoundingSphere<Real> FromBox( const BoundingBox<Real>& box );

#include <Math/MathUtil.inl>


} // Namespace RcEngine








#endif