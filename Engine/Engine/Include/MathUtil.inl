//----------------------------------------------------------------------------
template< typename Real, int Size >
inline static typename  Real
SquaredLength(const Vector<Real, Size>& vec)
{
	return Dot(vec, vec);
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
inline static typename Real
Length(const Vector<Real, Size>& vec)
{
	return (Real)sqrt((double)SquaredLength(vec));
}


//----------------------------------------------------------------------------
template< typename Real, int Size >
inline static typename Real
Dot( const Vector<Real, Size>& lhs, const Vector<Real, Size>& rhs )
{
	Real result = (Real)0;
	for(int i = 0; i < Size; i++)
		result +=  lhs[i] * rhs[i] ;
	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
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
inline void 
CreateLookAtMatrixLH(Matrix4<Real>& outMat,const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = MathUtil::Normalize(vAt - vEye);
	Vector<Real, 3> xAxis = MathUtil::Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = MathUtil::Cross(zAxis, xAxis);

	outMat.M11 = xAxis.X();
	outMat.M12 = yAxis.X();
	outMat.M13 = zAxis.X();
	outMat.M14 = (Real)0;

	outMat.M21 = xAxis.Y();
	outMat.M22 = yAxis.Y();
	outMat.M23 = zAxis.Y();
	outMat.M24 = (Real)0;

	outMat.M31 = xAxis.Z();
	outMat.M32 = yAxis.Z();
	outMat.M33 = zAxis.Z();
	outMat.M34 = (Real)0;

	outMat.M41 = - MathUtil::Dot(xAxis, vEye);
	outMat.M42 = - MathUtil::Dot(yAxis, vEye);
	outMat.M43 = - MathUtil::Dot(zAxis, vEye);
	outMat.M44 = (Real)1;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void
CreateLookAtMatrixRH(Matrix4<Real>& outMat,const Vector<Real,3>& vEye, const Vector<Real,3>& vAt, const Vector<Real,3>& vUp)
{
	Vector<Real, 3> zAxis = MathUtil::Normalize(vEye - vAt);
	Vector<Real, 3> xAxis = MathUtil::Normalize(Cross(vUp, zAxis));
	Vector<Real, 3> yAxis = MathUtil::Cross(zAxis, xAxis);

	outMat.M11 = xAxis.X();
	outMat.M12 = yAxis.X();
	outMat.M13 = zAxis.X();
	outMat.M14 = (Real)0;

	outMat.M21 = xAxis.Y();
	outMat.M22 = yAxis.Y();
	outMat.M23 = zAxis.Y();
	outMat.M24 = (Real)0;

	outMat.M31 = xAxis.Z();
	outMat.M32 = yAxis.Z();
	outMat.M33 = zAxis.Z();
	outMat.M34 = (Real)0;

	outMat.M41 = - MathUtil::Dot(xAxis, vEye);
	outMat.M42 = - MathUtil::Dot(yAxis, vEye);
	outMat.M43 = - MathUtil::Dot(zAxis, vEye);
	outMat.M44 = (Real)1;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreatePerspectiveFovLH(Matrix4<Real>& outMat, Real fovy, Real aspect, Real zNear,  Real zFar)
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
	
	outMat.M11 = xScale;
	outMat.M12 = (Real)0;
	outMat.M13 = (Real)0;
	outMat.M14 = (Real)0;

	outMat.M21 = (Real)0;
	outMat.M22 = yScale;
	outMat.M23 = (Real)0;
	outMat.M24 = (Real)0;

	outMat.M31 = (Real)0;
	outMat.M32 = (Real)0;
	outMat.M33 = zFar / (zFar - zNear);
	outMat.M34 = (Real)1;

	outMat.M41 = (Real)0;
	outMat.M42 = (Real)0;
	outMat.M43 =  zNear * zFar / (zNear - zFar);
	outMat.M44 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline void 
CreatePerspectiveFovRH(Matrix4<Real>& outMat, Real fovy, Real aspect,  Real zNear,  Real zFar)
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

	outMat.M11 = xScale;
	outMat.M12 = (Real)0;
	outMat.M13 = (Real)0;
	outMat.M14 = (Real)0;

	outMat.M21 = (Real)0;
	outMat.M22 = yScale;
	outMat.M23 = (Real)0;
	outMat.M24 = (Real)0;

	outMat.M31 = (Real)0;
	outMat.M32 = (Real)0;
	outMat.M33 = zFar / (zNear - zFar);
	outMat.M34 = (Real)-1;

	outMat.M41 = (Real)0;
	outMat.M42 = (Real)0;
	outMat.M43 =  zNear * zFar / (zNear - zFar);
	outMat.M44 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void
CreatePerspectiveLH(Matrix4<Real>& outMat, Real width, Real height, Real zNear,  Real zFar)
{
/*
	2*zn/w  0       0              0
	0       2*zn/h  0              0
	0       0       zf/(zf-zn)     1
	0       0       zn*zf/(zn-zf)  0
*/
	outMat.M11 = Real(2) * zNear / width;
	outMat.M12 = (Real)0;
	outMat.M13 = (Real)0;
	outMat.M14 = (Real)0;

	outMat.M21 = (Real)0;
	outMat.M22 = Real(2) * zNear / height;
	outMat.M23 = (Real)0;
	outMat.M24 = (Real)0;

	outMat.M31 = (Real)0;
	outMat.M32 = (Real)0;
	outMat.M33 = zFar / (zFar - zNear);
	outMat.M34 = (Real)1;

	outMat.M41 = (Real)0;
	outMat.M42 = (Real)0;
	outMat.M43 =  zNear * zFar / (zNear - zFar);
	outMat.M44 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreatePerspectiveRH(Matrix4<Real>& outMat, Real width, Real height, Real zNear,  Real zFar)
{
/*
	2*zn/w  0       0              0
	0       2*zn/h  0              0
	0       0       zf/(zn-zf)    -1
	0       0       zn*zf/(zn-zf)  0
*/
	outMat.M11 = Real(2) * zNear / width;
	outMat.M12 = (Real)0;
	outMat.M13 = (Real)0;
	outMat.M14 = (Real)0;

	outMat.M21 = (Real)0;
	outMat.M22 = Real(2) * zNear / height;
	outMat.M23 = (Real)0;
	outMat.M24 = (Real)0;

	outMat.M31 = (Real)0;
	outMat.M32 = (Real)0;
	outMat.M33 = zFar / (zNear - zFar);
	outMat.M34 = (Real)-1;

	outMat.M41 = (Real)0;
	outMat.M42 = (Real)0;
	outMat.M43 =  zNear * zFar / (zNear - zFar);
	outMat.M44 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline void 
CreateRotationX(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);
	
	outMat.M22 = outMat.M33 = cos;
	outMat.M23 = sin;
	outMat.M32 = -sin;

	outMat.M44 = outMat.M11 = (Real)1;

	outMat.M12 = outMat.M13 = outMat.M14 = outMat.M21 = 
	outMat.M24 = outMat.M31 = outMat.M34 = outMat.M41 = 
	outMat.M42 = outMat.M43 =  (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline void 
CreateRotationY(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);

	outMat.M13 = -sin;
	outMat.M31 = sin;
	outMat.M11 = outMat.M33 = cos;

	outMat.M44 = outMat.M22 = (Real)1;

	outMat.M12 = outMat.M14 = outMat.M21 = outMat.M23 = 
	outMat.M24 = outMat.M32 = outMat.M34 = outMat.M41 = 
	outMat.M42 = outMat.M43 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreateRotationZ(Matrix4<Real>& outMat, Real angle)
{
	Real cos = std::cos(angle);
	Real sin = std::sin(angle);

	outMat.M12 = sin;                          
	outMat.M21 = -sin;
	outMat.M11 = outMat.M22 = cos;


	outMat.M33 = outMat.M44 = (Real)1;               

	outMat.M13 = outMat.M14 = 
	outMat.M23 = outMat.M24 =
	outMat.M31 = outMat.M32 = outMat.M34 =
	outMat.M41 = outMat.M42 = outMat.M43 = (Real)0;
}


//----------------------------------------------------------------------------
template<typename Real>
inline  void
CreateRotationAxis(Matrix4<Real>& outMat,  const Vector<Real,3>& axis, Real angle)
{

}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreateScaling(Matrix4<Real>& outMat, Real sX, Real sY, Real sZ)
{
	outMat.M11 = sX;
	outMat.M22 = sY;
	outMat.M33 = sZ;
	outMat.M44 = (Real)1;


	outMat.M12 = outMat.M13 = outMat.M14 = outMat.M21 =
	outMat.M23 = outMat.M24 = outMat.M31 = outMat.M32 = 
	outMat.M34 = outMat.M41 = outMat.M42 = outMat.M43 = (Real)0;
}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreateTranslation(Matrix4<Real>& out, const Vector<Real,3>& trans)
{

}

//----------------------------------------------------------------------------
template<typename Real>
inline  void 
CreateTranslation(Matrix4<Real>& out, Real x, Real y, Real z)
{

}