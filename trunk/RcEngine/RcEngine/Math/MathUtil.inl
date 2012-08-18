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
MatrixInverse(const Matrix4<Real>& mat)
{
	Real v0 = mat.M31 * mat.M42 - mat.M32 * mat.M41;
	Real v1 = mat.M31 * mat.M43 - mat.M33 * mat.M41;
	Real v2 = mat.M31 * mat.M44 - mat.M34 * mat.M41;
	Real v3 = mat.M32 * mat.M43 - mat.M33 * mat.M42;
	Real v4 = mat.M32 * mat.M44 - mat.M34 * mat.M42;
	Real v5 = mat.M33 * mat.M44 - mat.M34 * mat.M43;

	Real i11 = (v5 * mat.M22 - v4 * mat.M23 + v3 * mat.M24);
	Real i21 = -(v5 * mat.M21 - v2 * mat.M23 + v1 * mat.M24);
	Real i31 = (v4 * mat.M21 - v2 * mat.M22 + v0 * mat.M24);
	Real i41 = -(v3 * mat.M21 - v1 * mat.M22 + v0 * mat.M23);

	Real invDet = 1.0f / (i11 * mat.M11 + i21 * mat.M12 + i31 * mat.M13 + i41 * mat.M14);

	i11 *= invDet;
	i21 *= invDet;
	i31 *= invDet;
	i41 *= invDet;

	Real i12 = -(v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i22 = (v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i32 = -(v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i42 = (v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	v0 = mat.M21 * mat.M42 - mat.M22 * mat.M41;
	v1 = mat.M21 * mat.M43 - mat.M23 * mat.M41;
	v2 = mat.M21 * mat.M44 - mat.M24 * mat.M41;
	v3 = mat.M22 * mat.M43 - mat.M23 * mat.M42;
	v4 = mat.M22 * mat.M44 - mat.M24 * mat.M42;
	v5 = mat.M23 * mat.M44 - mat.M24 * mat.M43;

	Real i13 = (v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i23 = -(v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i33 = (v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i43 = -(v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	v0 = mat.M32 * mat.M21 - mat.M31 * mat.M22;
	v1 = mat.M33 * mat.M21 - mat.M31 * mat.M23;
	v2 = mat.M34 * mat.M21 - mat.M31 * mat.M24;
	v3 = mat.M33 * mat.M22 - mat.M32 * mat.M23;
	v4 = mat.M34 * mat.M22 - mat.M32 * mat.M24;
	v5 = mat.M34 * mat.M23 - mat.M33 * mat.M24;

	Real i14 = -(v5 * mat.M12 - v4 * mat.M13 + v3 * mat.M14) * invDet;
	Real i24 = (v5 * mat.M11 - v2 * mat.M13 + v1 * mat.M14) * invDet;
	Real i34 = -(v4 * mat.M11 - v2 * mat.M12 + v0 * mat.M14) * invDet;
	Real i44 = (v3 * mat.M11 - v1 * mat.M12 + v0 * mat.M13) * invDet;

	return Matrix4<Real>(
		i11, i12, i13, i14,
		i21, i22, i23, i24,
		i31, i32, i33, i34,
		i41, i42, i43, i44);
}
//----------------------------------------------------------------------------

template<typename Real>
inline Matrix4<Real> 
CreateLookAtMatrixLH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = Normalize(vAt - vEye);
	Vector<Real, 3> xAxis = Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = Cross(zAxis, xAxis);

	return Matrix4<Real>(xAxis.X(), yAxis.X(), zAxis.X(), (Real)0, 
		xAxis.Y(), yAxis.Y(), zAxis.Y(), (Real)0, 
		xAxis.Z(), yAxis.Z(), zAxis.Z(), (Real)0,
		- Dot(xAxis, vEye), - Dot(yAxis, vEye), - Dot(zAxis, vEye), (Real)1);
}


//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateLookAtMatrixRH(const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = Normalize(vEye - vAt);
	Vector<Real, 3> xAxis = Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = Cross(zAxis, xAxis);

	return Matrix4<Real>(xAxis.X(), yAxis.X(), zAxis.X(), (Real)0, 
		xAxis.Y(), yAxis.Y(), zAxis.Y(), (Real)0, 
		xAxis.Z(), yAxis.Z(), zAxis.Z(), (Real)0,
		- Dot(xAxis, vEye), - Dot(yAxis, vEye), - Dot(zAxis, vEye), (Real)1);

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
CreateRotationX(Real angle)
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
CreateRotationY(Real angle)
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
CreateRotationZ(Real angle)
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
CreateRotationAxis(const Vector<Real,3>& axis, Real angle)
{
	const Real cosAngle = std::cos(angle);
	const Real sinAngle = std::sin(angle);	
	const Real lengthInv = (Real)1 / axis.Length();
	const Real nx = axis.X() * lengthInv;
	const Real ny = axis.Y() * lengthInv;
	const Real nz = axis.Z() * lengthInv;
	const Real oneMinusCos = (Real)1-cosAngle;

	return Matrix4<Real>(nx*nx*oneMinusCos+cosAngle,    nx*ny*oneMinusCos+nz*sinAngle, nx*nz*oneMinusCos-ny*sinAngle, (Real)0, 
		                 nx*ny*oneMinusCos-nz*sinAngle, ny*ny*oneMinusCos+cosAngle,    ny*nz*oneMinusCos+nx*sinAngle, (Real)0, 
		                 nx*nz*oneMinusCos+ny*sinAngle, ny*nz*oneMinusCos-nx*sinAngle, nz*nz*oneMinusCos+cosAngle,    (Real)0, 
						 (Real)0,                      (Real)0,                       (Real)0,                        (Real)1 );
}

//----------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateRotationYawPitchRoll(Real yaw, Real pitch, Real roll)
{
/**
 * The order of transformations is roll first, then pitch, then yaw. Relative to the object's local coordinate axis,
 * this is equivalent to rotation around the z-axis, followed by rotation around the x-axis, followed by rotation around 
 * the y-axis, as shown in the following illustration.
 * Equal to CreateRotationZ * CreateRotationX * CreateRotationY
 */ 
 
 const Real sinYaw = std::sin(yaw);
 const Real cosYaw = std::cos(yaw);
 const Real sinPitch = std::sin(pitch);
 const Real cosPitch =  std::cos(pitch);
 const Real sinRoll = std::sin(roll);
 const Real cosRoll = std::cos(roll); 

 return Matrix4<Real>( cosYaw*cosRoll+sinYaw*sinPitch*sinRoll,   sinRoll*cosPitch,   -sinYaw*cosRoll+cosYaw*sinPitch*sinRoll,  (Real)0, 
					   -cosYaw*sinRoll+sinYaw*sinPitch*cosRoll,  cosRoll*cosPitch,    sinRoll*sinYaw+cosYaw*sinPitch*cosRoll,  (Real)0, 
					   sinYaw*cosPitch,                          -sinPitch,           cosYaw*cosPitch,                          (Real)0,
					   (Real)0,                                 (Real)0,             (Real)0,                                  (Real)1 );
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

//----------------------------------------------------------------------------------------
template<typename Real>
inline Matrix4<Real> 
CreateTranslation(const Vector<Real,3>& trans)
{
	return Matrix4<Real>( (Real)1,    (Real)0,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)1,  (Real)0,   (Real)0, 
						  (Real)0,    (Real)0,  (Real)1,   (Real)0,
						 trans.X(),  trans.Y(), trans.Z(), (Real)1 );
}

//----------------------------------------------------------------------------------------
template<typename Real>
inline void 
RotationMatrixToYawPitchRoll(Real& yaw, Real& pitch, Real& roll, const Matrix4<Real>& mat)
{
/**
 * R = Rz * Ry * Rx 
 * | cosYcosR+sinYsinPsinR   sinRcosP    -sinYcosR+cosYsinPsinR |
 * | -cosYsinR+sinYsinPcosR  cosRcosP    sinRsinY+cosYsinPcosR  |
 * |   sinYcosP               -sinP      cosYcosP               |
 */
	
	// 从M32中直接接触Pitch
	Real sinPitch = -mat.M32;

	if (sinPitch <= Real(-0.999))
	{
		// 检查万向硕的情况，正在向上看
		pitch = Real(-PI/2);
		roll = 0;
		yaw = std::atan2(-mat.M13, mat.M11);
	}
	else if( sinPitch >= Real(0.999))
	{
		// 检查万向硕的情况，正在向下看
		pitch = Real(PI/2);
		roll = 0;
		yaw = std::atan2(-mat.M13, mat.M11);
	}
	else
	{
		pitch = std::asin(sinPitch);
		yaw = std::atan2(mat.M31, mat.M33);
		roll = std::atan2(mat.M12, mat.M22);
		
	}
}

//----------------------------------------------------------------------------------------
template<typename Real>
Vector<Real, 3> Transform(const Vector<Real, 3>& vec, const Matrix4<Real>& mat)
{
	Vector<Real, 4> result(vec.X(), vec.Y(), vec.Z(), (Real)1);
	result = result * mat;
	return Vector<Real, 3>(result.X(), result.Y(), result.Z());
}

template<typename Real>
Vector<Real, 3> TransformCoord(const Vector<Real, 3>& vec, const Matrix4<Real>& mat)
{
	Vector<Real, 4> result(vec.X(), vec.Y(), vec.Z(), (Real)1);
	result = result * mat;
	return Vector<Real, 3>(result.X() / result.W(), result.Y()/ result.W(), result.Z()/ result.W());
}

template<typename Real>
Vector<Real, 3> Transform(const Vector<Real, 3>& vec, const Quaternion<Real>& quat)
{
	Quaternion<Real> quatVec(Real(0), vec.X(), vec.Y(), vec.Z());
	Quaternion<Real> result = QuaternionInverse(quat) * quatVec * quat;
 	return Vector3f(result.X(), result.Y(), result.Z());
}


template<typename Real>
inline void 
MatrixDecompose(Vector<Real, 3>& sacle, Quaternion<Real>& rotation, Vector<Real, 3>& translation, const Matrix4<Real>& mat)
{
	translation.X() = mat.M41;
	translation.Y() = mat.M42;
	translation.Z() = mat.M43;

	Vector<Real,3> cols[] =
	{
		Vector<Real,3>(mat.M11, mat.M12, mat.M13),
		Vector<Real,3>(mat.M21, mat.M22, mat.M23),
		Vector<Real,3>(mat.M31, mat.M32, mat.M33)
	};

	sacle.X() = cols[0].Length();
	sacle.Y() = cols[1].Length();
	sacle.Z() = cols[2].Length();

	if (sacle.X() != 0)
	{
		cols[0] = cols[0] / sacle.X();
	}

	if (sacle.Y() != 0)
	{
		cols[1] = cols[1] / sacle.Y();
	}

	if (sacle.Z() != 0)
	{
		cols[2] = cols[2] / sacle.Z();
	}

	rotation = QuaternionFromRotationMatrix(Matrix4<Real>(
		cols[0].X(), cols[0].Y(), cols[0].Z(), (Real)0,
		cols[1].X(), cols[1].Y(), cols[1].Z(), (Real)0,
		cols[2].X(), cols[2].Y(), cols[2].Z(), (Real)0,
		(Real)0,     (Real)0,     (Real)0,     (Real)1
		));
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
inline Real QuaternionLength(const Quaternion<Real>& quat)
{
	return  std::sqrt(quat[0]*quat[0]+quat[1]*quat[1]+quat[2]*quat[2]+quat[3]*quat[3]);
}

template <typename Real>
inline Real
QuaternionDot(const Quaternion<Real>& quat)
{
	return quat[0]*quat[0]+quat[1]*quat[1]+quat[2]*quat[2]+quat[3]*quat[3];
}

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
Quaternion<Real> 
QuaternionInverse(const Quaternion<Real>& quat)
{
	Real inv = Real(1) / QuaternionLength(quat);
	return QuaternionConjugate(quat) * inv;
}

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationMatrix(const Matrix4<Real>& mat)
{
	Quaternion<Real> ret;
	
	Real trace = mat.M11 + mat.M22 + mat.M33;

	Real S;

	if (trace > 0) { 
		 S = std::sqrt(trace+(Real)1) * 2; // S=4*qw 
		ret.W() = (Real)0.25 * S;
		ret.X() = (mat.M23 - mat.M32) / S;
		ret.Y() = (mat.M31 - mat.M13) / S; 
		ret.Z() = (mat.M12 - mat.M21) / S; 
	} else if ((mat.M11 > mat.M22)&(mat.M11 > mat.M33)) { 
		 S = std::sqrt((Real)1.0 + mat.M11 - mat.M22 - mat.M33) * 2; // S=4*qx 
		ret.W() = (mat.M23 - mat.M32) / S;
		ret.X() = (Real)0.25 * S;
		ret.Y() = (mat.M21 + mat.M12) / S; 
		ret.Z() = (mat.M13 + mat.M31) / S; 
	} else if (mat.M22 > mat.M33) { 
		 S = std::sqrt((Real)1 + mat.M22 - mat.M11 - mat.M33) * 2; // S=4*qy
		ret.W() = (mat.M31 - mat.M13) / S;
		ret.X() = (mat.M12 + mat.M21) / S; 
		ret.Y() = (Real)0.25 * S;
		ret.Z() = (mat.M23 + mat.M32) / S; 
	} else { 
		 S = std::sqrt((Real)1 + mat.M33 - mat.M11 - mat.M22) * 2; // S=4*qz
		ret.W() = (mat.M12 - mat.M21) / S;
		ret.X() = (mat.M31 + mat.M13) / S;
		ret.Y() = (mat.M32 + mat.M23) / S;
		ret.Z() = (Real)0.25 * S;
	}

	/*Real s;
	if (trace > (Real)0)
	{
		s = std::sqrt((trace + (Real)1));
		ret.W() =s * (Real)0.5;
		s = (Real)0.5 / s;
		ret.X() = (mat.M23 - mat.M32) * s;
		ret.Y() = (mat.M31 - mat.M13) * s;
		ret.Z() = (mat.M12 - mat.M21) * s;
	}
	else
	{
		if ((mat.M22 > mat.M11) && (mat.M33 <= mat.M22))
		{
			s = std::sqrt((mat.M22 - (mat.M33 + mat.M11)) + (Real)1);
			ret.Y() = s * (Real)0.5;
			s = (Real)0.5 / s;
			ret.W() = (mat.M31 - mat.M13) * s;
			ret.Z() = (mat.M32 + mat.M23) * s;
			ret.X() = (mat.M12 + mat.M21) * s;
		}
		else
		{
			if (((mat.M22 <= mat.M11) && (mat.M33 > mat.M11)) || (mat.M33 > mat.M22))
			{
				s = sqrt((mat.M33 - (mat.M11 + mat.M22)) + 1);
				ret.Z() =s * (Real)0.5;
				s = (Real)0.5 / s;
				ret.W() = (mat.M12 - mat.M21) * s;
				ret.X() = (mat.M13 + mat.M31) * s;
				ret.Y() = (mat.M23 + mat.M32) * s;
			}
			else
			{
				s = sqrt((mat.M11 - (mat.M22 + mat.M33)) + 1);
				ret.X() = s * (Real)0.5;
				s = (Real)0.5 / s;
				ret.W() = (mat.M23 - mat.M32) * s;
				ret.Y() = (mat.M21 + mat.M12) * s;
				ret.Z() = (mat.M31 + mat.M13) * s;
			}
		}
	}*/
	return ret;
}

template <typename Real>
inline Matrix4<Real>
QuaternionToRotationMatrix(const Quaternion<Real>& rot)
{
	Real norm = (rot.W() * rot.W()) + (rot.X() * rot.X()) + (rot.Y() * rot.Y()) + (rot.Z() * rot.Z());
	Real s = (Real)0;
	//We must ensure the quaternion is normalized first. We also use 2 / norm to save mults later
	if(norm == (Real)1.0) {
		s = (Real)2;
	} else if(norm > 0.0f) {
		s = (Real)2 / norm;
	}

	//Compute xs/ys/zs since we use them 2-4 times, saves 6 mults
	float xs = rot.X() * s;
	float ys = rot.Y() * s;
	float zs = rot.Z() * s;

	float xx = rot.X() * xs;
	float xy = rot.X() * ys;
	float xz = rot.X() * zs;
	float xw = rot.W() * xs;

	float yy = rot.Y() * ys;
	float yz = rot.Y() * zs;
	float yw = rot.W() * ys;

	float zz = rot.Z() * zs;
	float zw = rot.W() * zs;

	return Matrix4<Real>(
		1-yy-zz,  xy+zw,   xz-yw,   (Real)0,
		xy-zw,    1-xx-zz, yz+xw,   (Real)0,
		xz+yw,    yz-xw,   1-xx-yy, (Real)0,
	    (Real)0,  (Real)0, (Real)0, (Real)1);
}

template <typename Real>
inline Quaternion<Real> 
QuaternionFromRotationAxis(const Vector<Real, 3>& axis, Real angleRadius)
{
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	Real halfAngle = ((Real)0.5)*angle;
	Real sn = std::sin(halfAngle); 
	return Quaternion<Real>(std::cos(halfAngle), sn*axis[0], sn*axis[1], sn*axis[2]);
}

template <typename Real>
inline void
QuaternionToAxisAngle(const Quaternion<Real>& quat, Vector<Real, 3>& axis, Real& angle)
{
	Real squareLength = quat[0]*quat[0] + quat[1]*quat[1] + quat[2]*quat[2] + quat[3]*quat[3];
	
	if (squareLength > (Real)(1e-06))
	{
		angle = ((Real)2.0)*std::acos(quat[0]);
		Real invLength = ((Real)1.0) / std::sqrt(squareLength);
		axis[0] = quat[1]*invLength;
		axis[1] = quat[2]*invLength;
		axis[2] = quat[3]*invLength;
	}
	else
	{
		// angle is 0 (mod 2*pi), so any axis will do
		angle = (Real)0.0;
		axis[0] = (Real)1.0;
		axis[1] = (Real)0.0;
		axis[2] = (Real)0.0;
	}

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

	return Quaternion<Real>(
		cosRoll * cosPitchCosYaw     + sinRoll * sinPitchSinYaw,
		cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
		cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
		sinRoll * cosPitchCosYaw     - cosRoll * sinPitchSinYaw);	
}

template <typename Real>
inline void 
QuaternionToRotationYawPitchRoll(Real& yaw, Real& pitch, Real& roll, const Quaternion<Real>& quat)
{
	RotationMatrixToYawPitchRoll(yaw, pitch, roll, QuaternionToRotationMatrix(quat));
	//Real sqx = quat.X()*quat.X();
	//Real sqy = quat.Y()*quat.Y();
	//Real sqz = quat.Z()*quat.Z();
	//Real sqw = quat.W()*quat.W();
	//Real unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	//
	//// sinPitch 
	//Real test = Real(-2.0) * ( quat.Y() * quat.Z() - quat.X() * quat.W() );

	//if(test >  Real(0.999) * unit)
	//{
	//	// singularity at north pole
	//	yaw = std::atan2(quat.W()*quat.Y()-quat.X()*quat.Z(), Real(0.5)-quat.Y()*quat.Y()-quat.Z()*quat.Z());
	//	pitch = Real(PI/2);
	//	roll = 0;
	//}
	//else if(test <  Real(-0.999) * unit)
	//{
	//	// singularity at south pole
	//	yaw = -2 * std::atan2(quat.Z(), quat.W());
	//	pitch = -Real(PI/2);
	//	roll = 0;
	//}
	//else
	//{
	//	yaw = atan2((quat.Y() * quat.W() + quat.X() * quat.Z()), -sqx - sqy + sqz + sqw);
	//	pitch = asin(test / unit);
	//	roll = atan2((quat.Z() * quat.W() + quat.X() * quat.Y()), -sqx + sqy - sqz + sqw);
	//}
	//	

	//if (sinPitch <= Real(-0.999))
	//{
	//	// 检查万向硕的情况，正在向上看
	//	pitch = Real(-PI/2);
	//	roll = 0;
	//	yaw = std::atan2(-mat.M13, mat.M11);
	//}
	//else if( sinPitch >= Real(0.999))
	//{
	//	// 检查万向硕的情况，正在向下看
	//	pitch = Real(PI/2);
	//	roll = 0;
	//	yaw = std::atan2(-mat.M13, mat.M11);
	//}
	//else
	//{
	//	pitch = std::asin(sinPitch);
	//	yaw = std::atan2(mat.M31, mat.M33);
	//	roll = std::atan2(mat.M12, mat.M22);

	//}
		
	//	(quat.X() * quat.W() + quat.Y() * quat.Z();

	//if(test >  Real(0.499) * unit)
	//{
	//	// singularity at north pole
	//	yaw = 2 * std::atan2(quat.Z(), quat.W());
	//	pitch = Math::PI / 2;
	//	roll = 0;
	//}
	//else if(test <  Real(-0.499) * unit)
	//{
	//	// singularity at south pole
	//	yaw = -2 * std::atan2(quat.Z(), quat.W());
	//	pitch = -Math::PI / 2;
	//	roll = 0;
	//}
	//else
	//{
	//	yaw = atan2(2 * (quat.Y() * quat.W() - quat.X() * quat.Z()), -sqx - sqy + sqz + sqw);
	//	pitch = asin(2 * test / unit);
	//	roll = atan2(2 * (quat.Z() * quat.W() - quat.X() * quat.Y()), -sqx + sqy - sqz + sqw);
	//}
}


template <typename Real>
inline Quaternion<Real> 
QuaternionSlerp(const Quaternion<Real>& quat1, const Quaternion<Real>& quat2, Real t)
{
	// 用点乘计算两四元素夹角的cos值
	Real cosOmega = quat1[0]*quat2[0] + quat1[1]*quat2[1] + quat1[2]*quat2[2] + quat1[3]*quat2[3];
	
	Real dir = Real(1);

	//如果点乘为负，则反转一个四元素以取得短的4D弧
	if( cosOmega < Real(0) )
	{
		dir = Real(-1);
		cosOmega = -cosOmega;
	}
	
	Real scale1, scale2;

	// 检查是否过于接近以避免除零
	if( cosOmega < 1 - std::numeric_limits<Real>::epsilon() )
	{
		Real omega = std::acos(cosOmega);
		Real oneOverSinOmega = Real(1) / std::sin(omega);

		scale1 = std::sin( (Real(1) - t)*omega ) * oneOverSinOmega;
		scale2 = std::sin(t * omega) * oneOverSinOmega;
	}
	else
	{
		//非常近，线性插值
		scale1 = Real(1) - t;
		scale2 = t;
	}

	return quat1 * scale1 + quat2 *  dir * scale2;
}


//////////////////////////////////////////////////////////////////////////



