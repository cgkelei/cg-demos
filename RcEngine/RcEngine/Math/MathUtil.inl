//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline static typename  Real
SquaredLength(const Vector<Real, Size>& vec)
{
	return Dot(vec, vec);
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline static typename Real
Length(const Vector<Real, Size>& vec)
{
	return std::sqrt(SquaredLength(vec));
}


//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline static typename Real
Dot( const Vector<Real, Size>& lhs, const Vector<Real, Size>& rhs )
{
	Real result = (Real)0;
	for(int32_t i = 0; i < Size; i++)
		result +=  lhs[i] * rhs[i] ;
	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int32_t Size >
inline static Vector<Real, Size>
Normalize(const Vector<Real, Size>& vec)
{
	Real fLength = SquaredLength(vec);
	Real fInvScalar = ((Real)1.0)/ ((Real)sqrt((double)fLength));
	return vec * fInvScalar;
}

//----------------------------------------------------------------------------
template<typename Real>
inline Vector<Real, 3>
Cross( const Vector<Real, 3>& vec1, const Vector<Real, 3>& vec2 )
{
	return Vector<Real, 3>(
		vec1.Y() * vec2.Z() - vec1.Z() * vec2.Y(),
		vec1.Z() * vec2.X() - vec1.X() * vec2.Z(), 
		vec1.X() * vec2.Y() - vec1.Y() * vec2.X());
}


//----------------------------------------------------------------------------
template< typename Real >
inline Plane3<Real>
Normalize(const Plane3<Real>& plane)
{
	return Plane3<Real>(Normalize(plane.Normal), plane.Constant);
}


//----------------------------------------------------------------------------

template<typename Real>
inline Matrix4<Real> 
CreateLookAtMatrixLH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = MathUtil::Normalize(vAt - vEye);
	Vector<Real, 3> xAxis = MathUtil::Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = MathUtil::Cross(zAxis, xAxis);

	return Matrix4<Real>(xAxis.X(), yAxis.X(), zAxis.X(), (Real)0, 
		xAxis.Y(), yAxis.Y(), zAxis.Y(), (Real)0, 
		xAxis.Z(), yAxis.Z(), zAxis.Z(), (Real)0,
		- MathUtil::Dot(xAxis, vEye), - MathUtil::Dot(yAxis, vEye), - MathUtil::Dot(zAxis, vEye), (Real)1);
}


//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateLookAtMatrixRH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = MathUtil::Normalize(vEye - vAt);
	Vector<Real, 3> xAxis = MathUtil::Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = MathUtil::Cross(zAxis, xAxis);

	return Matrix4<Real>(xAxis.X(), yAxis.X(), zAxis.X(), (Real)0, 
		xAxis.Y(), yAxis.Y(), zAxis.Y(), (Real)0, 
		xAxis.Z(), yAxis.Z(), zAxis.Z(), (Real)0,
		- MathUtil::Dot(xAxis, vEye), - MathUtil::Dot(yAxis, vEye), - MathUtil::Dot(zAxis, vEye), (Real)1);

}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreatePerspectiveFovLH(Real fovy, Real aspect, Real zNear,  Real zFar)
{
/*
	xScale     0          0               0
	0        yScale       0               0
	0          0       zf/(zf-zn)         1
	0          0       -zn*zf/(zf-zn)     0
	where:
	yScale = cot(fovY/2)

	xScale = yScale / aspect ratio
*/
	Real yScale = Real(1) / (std::tan(fovy / 2));
	Real xScale = yScale / aspect;
	
	return Matrix4<Real>(xScale, (Real)0, (Real)0, (Real)0, 
		(Real)0, yScale, (Real)0, (Real)0, 
		(Real)0, (Real)0, zFar / (zFar - zNear), (Real)1,
		(Real)0, (Real)0, zNear * zFar / (zNear - zFar),(Real)0);
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreatePerspectiveFovRH(Real fovy, Real aspect,  Real zNear,  Real zFar)
{
/*
	xScale     0          0              0
	0        yScale       0              0
	0          0      zf/(zn-zf)        -1
	0          0      zn*zf/(zn-zf)      0
	where:
	yScale = cot(fovY/2)

	xScale = yScale / aspect ratio
*/
	Real yScale = Real(1) / (std::tan(fovy / 2));
	Real xScale = yScale / aspect;

	return Matrix4<Real>(xScale, (Real)0, (Real)0, (Real)0, 
		(Real)0, yScale, (Real)0, (Real)0, 
		(Real)0, (Real)0, zFar / (zNear - zFar), (Real)-1,
		(Real)0, (Real)0, zNear * zFar / (zNear - zFar),(Real)0);

}


//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateRotationX(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);
	
	return Matrix4<Real>((Real)1, (Real)0, (Real)0, (Real)0, 
						 (Real)0,  cos,     sin,    (Real)0, 
						 (Real)0,  -sin,    cos,    (Real)0,
						 (Real)0, (Real)0, (Real)0, (Real)1 );
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateRotationY(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);

	return Matrix4<Real>( cos,   (Real)0,  -sin,   (Real)0, 
		                (Real)0, (Real)1, (Real)0, (Real)0, 
		                  sin,   (Real)0,  cos,    (Real)0,
		                (Real)0, (Real)0, (Real)0, (Real)1 );
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateRotationZ(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);

	return Matrix4<Real>( cos,   sin,    (Real)0,  (Real)0, 
					     -sin,   cos,    (Real)0,  (Real)0, 
					   (Real)0, (Real)0, (Real)1,  (Real)0,
					   (Real)0, (Real)0, (Real)0,  (Real)1 );
}


//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateRotationAxis(Matrix4<Real>& outMat,  const Vector<Real,3>& axis, Real angle)
{

}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateScaling(Real sX, Real sY, Real sZ)
{
	return Matrix4<Real>( sX,    (Real)0, (Real)0,  (Real)0, 
					    (Real)0,  sY,     (Real)0,  (Real)0, 
					    (Real)0, (Real)0,   sZ,     (Real)0,
						(Real)0, (Real)0, (Real)0,  (Real)1 );
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateTranslation(const Vector<Real,3>& trans)
{
	return Matrix4<Real>( (Real)1,    (Real)0,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)1,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)0,  (Real)1,   (Real)0,
						 trans.X(),  trans.Y(), trans.Z(), (Real)1 );
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateTranslation(Real x, Real y, Real z)
{
	return Matrix4<Real>( (Real)1,    (Real)0,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)1,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)0,  (Real)1,   (Real)0,
							x,          y,        z,       (Real)1 );

}

//----------------------------------------------------------------------------
template <typename Real>
inline Quaternion<Real> 
QuaternionNormalize(const Quaternion<Real>& quat)
{
	Real mag = std::sqrt(quat[0]*quat[0]+quat[1]*quat[1]+quat[2]*quat[2]+quat[3]*quat[3]);
	Real oneOverMag = Real(1) / mag;	//暂时不检查除0;
	return Quaternion<Real>(quat[0]*oneOverMag, quat[1]*oneOverMag, 
		quat[2]*oneOverMag, quat[3]*oneOverMag);
}

template <typename Real>
inline Quaternion<Real> 
QuaternionConjugate(const Quaternion<Real>& quat)
{
	return Quaternion<Real>(quat.W(), -quat.X(), -quat.Y(), -quat.Z());
}

template <typename Real>
inline Quaternion<Real> 
	QuaternionMultiply(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2)
{
	return Quaternion<Real>(
		quat1[0]*quat2[0] - quat1[1]*quat2[1] - quat1[2]*quat2[2] - quat1[3]*quat2[3],
		quat1[0]*quat2[1] + quat1[1]*quat2[0] + quat1[2]*quat2[3] - quat1[3]*quat2[2],
		quat1[0]*quat2[2] + quat1[2]*quat2[0] + quat1[3]*quat2[1] - quat1[1]*quat2[3],
		quat1[0]*quat2[3] + quat1[3]*quat2[0] + quat1[1]*quat2[2] - quat1[2]*quat2[1] );

}

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationMatrix(const Matrix4<Real>& rotMat)
{
	return Quaternion<Real>();
}

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationAxis(const Vector<Real, 3>& axis, Real angleRadius)
{
	Real halfAngle = ((Real)0.5)*angle;
	Real sn = std::sin(halfAngle); 
	return Quaternion<Real>(std::cos(halfAngle), sn*axis[0], sn*axis[1], sn*axis[2]);
}

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationYawPitchRoll(Real yaw, Real pitch, Real roll)
{
	const Real sinPitch(std::sin(pitch*((Real)0.5)));
	const Real cosPitch(std::cos(pitch*((Real)0.5)));
	const Real sinYaw(std::sin(yaw*((Real)0.5)));
	const Real cosYaw(std::cos(yaw*((Real)0.5)));
	const Real sinRoll(std::sin(roll*((Real)0.5)));
	const Real cosRoll(std::cos(roll*((Real)0.5)));
	const Real cosPitchCosYaw(cosPitch*cosYaw);
	const Real sinPitchSinYaw(sinPitch*sinYaw);

	return return Quaternion<Real>(
		cosRoll * cosPitchCosYaw     + sinRoll * sinPitchSinYaw,
		sinRoll * cosPitchCosYaw     - cosRoll * sinPitchSinYaw,
		cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
		cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw);	
}
//
//template <typename Real>
//inline Quaternion<Real> 
//QuaternionSlerp(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2, Real t)
//{
//	// 用点乘计算两四元素夹角的cos值
//	Real cosOmega = quat1[0]*quat2[0] + quat1[1]*quat2[1] + quat1[2]*quat2[2] + quat1[3]*quat2[3];
//	
//	Real dir = Real(1);
//
//	//如果点乘为负，则反转一个四元素以取得短的4D弧
//	if( cosOmega < Real(0) )
//	{
//		dir = Real(-1);
//		cosOmega = -cosOmega;
//	}
//	
//	Real scale1, scale2;
//
//	// 检查是否过于接近以避免除零
//	if( cosOmega < 1 - std::numeric_limits<Real>::epsilon() )
//	{
//		Real omega = std::acos(cosOmega);
//		Real oneOverSinOmega = Real(1) / std::sin(omega);
//
//		scale1 = std::sin( (Real(1) - t)*omega ) * oneOverSinOmega;
//		scale2 = std::sin(t * omega) * oneOverSinOmega;
//	}
//	else
//	{
//		//非常近，线性插值
//		scale1 = Real(1) - t;
//		scale2 = t;
//	}
//
//	return quat1 * scale1 + quat2 *  dir * scale2;
//}


template <typename Real>
inline void
QuaternionToAxisAngle(const Quaternion<Real>& quat, Vector<Real, 3>& axis, Real& angle)
{
	Real halfAngle = 

}

//////////////////////////////////////////////////////////////////////////


