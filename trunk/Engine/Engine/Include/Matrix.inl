//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4()
{
	 MakeZero();
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4(const Real* rhs)
{
	memcpy(Elements, rhs, sizeof(Elements));
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4(const Matrix4<Real>& rhs)
{
	memcpy(Elements, rhs.Elements, sizeof(Elements));
}

//----------------------------------------------------------------------------
template<typename Real> 
template<typename T>
Matrix4<Real>::Matrix4(const Matrix4<T>& rhs)
{
	for(int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			Elements[MatrixItem(i, j)] = static_cast<Real>(rhs.Elements[MatrixItem(i, j))];
		}

}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>::Matrix4( Real f11, Real f12, Real f13, Real f14,
					 Real f21, Real f22, Real f23, Real f24,
					 Real f31, Real f32, Real f33, Real f34,
					 Real f41, Real f42, Real f43, Real f44 )
{
	M11	= f11; M12 = f12; M13 = f13; M14 = f14; 
	M21	= f21; M22 = f22; M23 = f23; M24 = f24; 
	M31	= f31; M32 = f32; M33 = f33; M34 = f34; 
	M41	= f41; M42 = f42; M43 = f43; M44 = f44; 
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::MakeIdentity()
{
	Elements[ 0] = (Real)1.0;
	Elements[ 1] = (Real)0.0;
	Elements[ 2] = (Real)0.0;
	Elements[ 3] = (Real)0.0;
	Elements[ 4] = (Real)0.0;
	Elements[ 5] = (Real)1.0;
	Elements[ 6] = (Real)0.0;
	Elements[ 7] = (Real)0.0;
	Elements[ 8] = (Real)0.0;
	Elements[ 9] = (Real)0.0;
	Elements[10] = (Real)1.0;
	Elements[11] = (Real)0.0;
	Elements[12] = (Real)0.0;
	Elements[13] = (Real)0.0;
	Elements[14] = (Real)0.0;
	Elements[15] = (Real)1.0;
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::MakeZero()
{
	Elements[ 0] = (Real)0.0;
	Elements[ 1] = (Real)0.0;
	Elements[ 2] = (Real)0.0;
	Elements[ 3] = (Real)0.0;
	Elements[ 4] = (Real)0.0;
	Elements[ 5] = (Real)0.0;
	Elements[ 6] = (Real)0.0;
	Elements[ 7] = (Real)0.0;
	Elements[ 8] = (Real)0.0;
	Elements[ 9] = (Real)0.0;
	Elements[10] = (Real)0.0;
	Elements[11] = (Real)0.0;
	Elements[12] = (Real)0.0;
	Elements[13] = (Real)0.0;
	Elements[14] = (Real)0.0;
	Elements[15] = (Real)0.0;
}

//----------------------------------------------------------------------------
template<typename Real>
const Real* Matrix4<Real>::operator()() const
{
	return Elements;
}	

//----------------------------------------------------------------------------
template<typename Real>
Real* Matrix4<Real>::operator()()
{
	return Elements;
}

//----------------------------------------------------------------------------
template<typename Real>
const Real* Matrix4<Real>::operator[]( int iRow ) const
{
	return &Elements[4*iRow];
}

//----------------------------------------------------------------------------
template<typename Real>
Real* Engine::Matrix4<Real>::operator[]( int iRow )
{
	return &Elements[4*iRow];
}

//----------------------------------------------------------------------------
template<typename Real>
Real Matrix4<Real>::operator()( int iRow, int iCol ) const
{
	return Elements[MatrixItem(iRow, iCol)];
}

//----------------------------------------------------------------------------
template<typename Real>
Real& Matrix4<Real>::operator()( int iRow, int iCol )
{
	return Elements[MatrixItem(iRow, iCol)];
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::SetRow( int iRow, const Vector<Real, 4>& rhs )
{
	Elements[MatrixItem(iRow, 0)] = rhs[0];
	Elements[MatrixItem(iRow, 1)] = rhs[1];
	Elements[MatrixItem(iRow, 2)] = rhs[2];
	Elements[MatrixItem(iRow, 3)] = rhs[3];
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::GetRow( int iRow ) const
{
	return Vector<Real, 4>(Elements[MatrixItem(iRow, 0)], Elements[MatrixItem(iRow, 1)],
				Elements[MatrixItem(iRow, 2)], Elements[MatrixItem(iRow, 3)]);
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::SetColumn( int iCol, const Vector<Real, 4>& rhs )
{
	Elements[MatrixItem(0, iCol)] = rhs[0];
	Elements[MatrixItem(1, iCol)]= rhs[1];
	Elements[MatrixItem(2, iCol)] = rhs[2];
	Elements[MatrixItem(3, iCol)] = rhs[3];
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::GetColumn( int iCol ) const
{
	return Vector<Real, 4>(Elements[MatrixItem(0, iCol)], Elements[MatrixItem(1, iCol)],
		Elements[MatrixItem(2, iCol)], Elements[MatrixItem(3, iCol)]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator=( const Matrix4<Real>& rhs )
{
	Elements[ 0] = rhs.Elements[ 0];
	Elements[ 1] = rhs.Elements[ 1];
	Elements[ 2] = rhs.Elements[ 2];
	Elements[ 3] = rhs.Elements[ 3];
	Elements[ 4] = rhs.Elements[ 4];
	Elements[ 5] = rhs.Elements[ 5];
	Elements[ 6] = rhs.Elements[ 6];
	Elements[ 7] = rhs.Elements[ 7];
	Elements[ 8] = rhs.Elements[ 8];
	Elements[ 9] = rhs.Elements[ 9];
	Elements[10] = rhs.Elements[10];
	Elements[11] = rhs.Elements[11];
	Elements[12] = rhs.Elements[12];
	Elements[13] = rhs.Elements[13];
	Elements[14] = rhs.Elements[14];
	Elements[15] = rhs.Elements[15];
	return *this;
}


//----------------------------------------------------------------------------
template<typename Real>
int Matrix4<Real>::CompareArrays (const Matrix4<Real>& rhs) const
{
	return memcmp(Elements,rhs.Elements,16*sizeof(Real));
}

//----------------------------------------------------------------------------
template<typename Real>
bool Matrix4<Real>::operator==( const Matrix4<Real>& rhs ) const
{
	return CompareArrays(rhs) == 0;
}

//----------------------------------------------------------------------------
template<typename Real>
bool Matrix4<Real>::operator!=( const Matrix4<Real>& rhs ) const
{
	return CompareArrays(rhs) != 0;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator+( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0] + rhs.Elements[ 0],
		Elements[ 1] + rhs.Elements[ 1],
		Elements[ 2] + rhs.Elements[ 2],
		Elements[ 3] + rhs.Elements[ 3],
		Elements[ 4] + rhs.Elements[ 4],
		Elements[ 5] + rhs.Elements[ 5],
		Elements[ 6] + rhs.Elements[ 6],
		Elements[ 7] + rhs.Elements[ 7],
		Elements[ 8] + rhs.Elements[ 8],
		Elements[ 9] + rhs.Elements[ 9],
		Elements[10] + rhs.Elements[10],
		Elements[11] + rhs.Elements[11],
		Elements[12] + rhs.Elements[12],
		Elements[13] + rhs.Elements[13],
		Elements[14] + rhs.Elements[14],
		Elements[15] + rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator-( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0] - rhs.Elements[ 0],
		Elements[ 1] - rhs.Elements[ 1],
		Elements[ 2] - rhs.Elements[ 2],
		Elements[ 3] - rhs.Elements[ 3],
		Elements[ 4] - rhs.Elements[ 4],
		Elements[ 5] - rhs.Elements[ 5],
		Elements[ 6] - rhs.Elements[ 6],
		Elements[ 7] - rhs.Elements[ 7],
		Elements[ 8] - rhs.Elements[ 8],
		Elements[ 9] - rhs.Elements[ 9],
		Elements[10] - rhs.Elements[10],
		Elements[11] - rhs.Elements[11],
		Elements[12] - rhs.Elements[12],
		Elements[13] - rhs.Elements[13],
		Elements[14] - rhs.Elements[14],
		Elements[15] - rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator*( const Matrix4<Real>& rhs ) const
{
	return Matrix4<Real>(
		Elements[ 0]*rhs.Elements[ 0] +
		Elements[ 1]*rhs.Elements[ 4] +
		Elements[ 2]*rhs.Elements[ 8] +
		Elements[ 3]*rhs.Elements[12],

		Elements[ 0]*rhs.Elements[ 1] +
		Elements[ 1]*rhs.Elements[ 5] +
		Elements[ 2]*rhs.Elements[ 9] +
		Elements[ 3]*rhs.Elements[13],

		Elements[ 0]*rhs.Elements[ 2] +
		Elements[ 1]*rhs.Elements[ 6] +
		Elements[ 2]*rhs.Elements[10] +
		Elements[ 3]*rhs.Elements[14],

		Elements[ 0]*rhs.Elements[ 3] +
		Elements[ 1]*rhs.Elements[ 7] +
		Elements[ 2]*rhs.Elements[11] +
		Elements[ 3]*rhs.Elements[15],

		Elements[ 4]*rhs.Elements[ 0] +
		Elements[ 5]*rhs.Elements[ 4] +
		Elements[ 6]*rhs.Elements[ 8] +
		Elements[ 7]*rhs.Elements[12],

		Elements[ 4]*rhs.Elements[ 1] +
		Elements[ 5]*rhs.Elements[ 5] +
		Elements[ 6]*rhs.Elements[ 9] +
		Elements[ 7]*rhs.Elements[13],

		Elements[ 4]*rhs.Elements[ 2] +
		Elements[ 5]*rhs.Elements[ 6] +
		Elements[ 6]*rhs.Elements[10] +
		Elements[ 7]*rhs.Elements[14],

		Elements[ 4]*rhs.Elements[ 3] +
		Elements[ 5]*rhs.Elements[ 7] +
		Elements[ 6]*rhs.Elements[11] +
		Elements[ 7]*rhs.Elements[15],

		Elements[ 8]*rhs.Elements[ 0] +
		Elements[ 9]*rhs.Elements[ 4] +
		Elements[10]*rhs.Elements[ 8] +
		Elements[11]*rhs.Elements[12],

		Elements[ 8]*rhs.Elements[ 1] +
		Elements[ 9]*rhs.Elements[ 5] +
		Elements[10]*rhs.Elements[ 9] +
		Elements[11]*rhs.Elements[13],

		Elements[ 8]*rhs.Elements[ 2] +
		Elements[ 9]*rhs.Elements[ 6] +
		Elements[10]*rhs.Elements[10] +
		Elements[11]*rhs.Elements[14],

		Elements[ 8]*rhs.Elements[ 3] +
		Elements[ 9]*rhs.Elements[ 7] +
		Elements[10]*rhs.Elements[11] +
		Elements[11]*rhs.Elements[15],

		Elements[12]*rhs.Elements[ 0] +
		Elements[13]*rhs.Elements[ 4] +
		Elements[14]*rhs.Elements[ 8] +
		Elements[15]*rhs.Elements[12],

		Elements[12]*rhs.Elements[ 1] +
		Elements[13]*rhs.Elements[ 5] +
		Elements[14]*rhs.Elements[ 9] +
		Elements[15]*rhs.Elements[13],

		Elements[12]*rhs.Elements[ 2] +
		Elements[13]*rhs.Elements[ 6] +
		Elements[14]*rhs.Elements[10] +
		Elements[15]*rhs.Elements[14],

		Elements[12]*rhs.Elements[ 3] +
		Elements[13]*rhs.Elements[ 7] +
		Elements[14]*rhs.Elements[11] +
		Elements[15]*rhs.Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator*( Real fScalar ) const
{
	return Matrix4<Real>(
		fScalar*Elements[ 0],
		fScalar*Elements[ 1],
		fScalar*Elements[ 2],
		fScalar*Elements[ 3],
		fScalar*Elements[ 4],
		fScalar*Elements[ 5],
		fScalar*Elements[ 6],
		fScalar*Elements[ 7],
		fScalar*Elements[ 8],
		fScalar*Elements[ 9],
		fScalar*Elements[10],
		fScalar*Elements[11],
		fScalar*Elements[12],
		fScalar*Elements[13],
		fScalar*Elements[14],
		fScalar*Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator/( Real fScalar ) const
{
	Real fInvScalar = ((Real)1.0)/fScalar;
	return Matrix4<Real>(
		fInvScalar*Elements[ 0],
		fInvScalar*Elements[ 1],
		fInvScalar*Elements[ 2],
		fInvScalar*Elements[ 3],
		fInvScalar*Elements[ 4],
		fInvScalar*Elements[ 5],
		fInvScalar*Elements[ 6],
		fInvScalar*Elements[ 7],
		fInvScalar*Elements[ 8],
		fInvScalar*Elements[ 9],
		fInvScalar*Elements[10],
		fInvScalar*Elements[11],
		fInvScalar*Elements[12],
		fInvScalar*Elements[13],
		fInvScalar*Elements[14],
		fInvScalar*Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::operator-() const
{
	return Matrix4<Real>(
		-Elements[ 0],
		-Elements[ 1],
		-Elements[ 2],
		-Elements[ 3],
		-Elements[ 4],
		-Elements[ 5],
		-Elements[ 6],
		-Elements[ 7],
		-Elements[ 8],
		-Elements[ 9],
		-Elements[10],
		-Elements[11],
		-Elements[12],
		-Elements[13],
		-Elements[14],
		-Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator+=( const Matrix4<Real>& rhs )
{
	Elements[ 0] += rhs.Elements[ 0];
	Elements[ 1] += rhs.Elements[ 1];
	Elements[ 2] += rhs.Elements[ 2];
	Elements[ 3] += rhs.Elements[ 3];
	Elements[ 4] += rhs.Elements[ 4];
	Elements[ 5] += rhs.Elements[ 5];
	Elements[ 6] += rhs.Elements[ 6];
	Elements[ 7] += rhs.Elements[ 7];
	Elements[ 8] += rhs.Elements[ 8];
	Elements[ 9] += rhs.Elements[ 9];
	Elements[10] += rhs.Elements[10];
	Elements[11] += rhs.Elements[11];
	Elements[12] += rhs.Elements[12];
	Elements[13] += rhs.Elements[13];
	Elements[14] += rhs.Elements[14];
	Elements[15] += rhs.Elements[15];
	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator-=( const Matrix4<Real>& rhs )
{
	Elements[ 0] -= rhs.Elements[ 0];
	Elements[ 1] -= rhs.Elements[ 1];
	Elements[ 2] -= rhs.Elements[ 2];
	Elements[ 3] -= rhs.Elements[ 3];
	Elements[ 4] -= rhs.Elements[ 4];
	Elements[ 5] -= rhs.Elements[ 5];
	Elements[ 6] -= rhs.Elements[ 6];
	Elements[ 7] -= rhs.Elements[ 7];
	Elements[ 8] -= rhs.Elements[ 8];
	Elements[ 9] -= rhs.Elements[ 9];
	Elements[10] -= rhs.Elements[10];
	Elements[11] -= rhs.Elements[11];
	Elements[12] -= rhs.Elements[12];
	Elements[13] -= rhs.Elements[13];
	Elements[14] -= rhs.Elements[14];
	Elements[15] -= rhs.Elements[15];
	return *this;
}


//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator*=(Real fScalar)
{
	Elements[ 0] *= fScalar;
	Elements[ 1] *= fScalar;
	Elements[ 2] *= fScalar;
	Elements[ 3] *= fScalar;
	Elements[ 4] *= fScalar;
	Elements[ 5] *= fScalar;
	Elements[ 6] *= fScalar;
	Elements[ 7] *= fScalar;
	Elements[ 8] *= fScalar;
	Elements[ 9] *= fScalar;
	Elements[10] *= fScalar;
	Elements[11] *= fScalar;
	Elements[12] *= fScalar;
	Elements[13] *= fScalar;
	Elements[14] *= fScalar;
	Elements[15] *= fScalar;
	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real>& Matrix4<Real>::operator/=(Real fScalar)
{
	Real fInvScalar = ((Real)1.0)/fScalar;
	Elements[ 0] *= fInvScalar;
	Elements[ 1] *= fInvScalar;
	Elements[ 2] *= fInvScalar;
	Elements[ 3] *= fInvScalar;
	Elements[ 4] *= fInvScalar;
	Elements[ 5] *= fInvScalar;
	Elements[ 6] *= fInvScalar;
	Elements[ 7] *= fInvScalar;
	Elements[ 8] *= fInvScalar;
	Elements[ 9] *= fInvScalar;
	Elements[10] *= fInvScalar;
	Elements[11] *= fInvScalar;
	Elements[12] *= fInvScalar;
	Elements[13] *= fInvScalar;
	Elements[14] *= fInvScalar;
	Elements[15] *= fInvScalar;

	return *this;
}

//----------------------------------------------------------------------------
template<typename Real>
Vector<Real, 4> Matrix4<Real>::operator*( const Vector<Real, 4>& rhs ) const
{
	return Vector4<Real>(
		Elements[ 0]*rhs[0] +
		Elements[ 1]*rhs[1] +
		Elements[ 2]*rhs[2] +
		Elements[ 3]*rhs[3],

		Elements[ 4]*rhs[0] +
		Elements[ 5]*rhs[1] +
		Elements[ 6]*rhs[2] +
		Elements[ 7]*rhs[3],

		Elements[ 8]*rhs[0] +
		Elements[ 9]*rhs[1] +
		Elements[10]*rhs[2] +
		Elements[11]*rhs[3],

		Elements[12]*rhs[0] +
		Elements[13]*rhs[1] +
		Elements[14]*rhs[2] +
		Elements[15]*rhs[3]);
}

//----------------------------------------------------------------------------
template<typename Real>
Matrix4<Real> Matrix4<Real>::GetTransposed() const
{
	return Matrix4<Real>(
		Elements[ 0],
		Elements[ 4],
		Elements[ 8],
		Elements[12],
		Elements[ 1],
		Elements[ 5],
		Elements[ 9],
		Elements[13],
		Elements[ 2],
		Elements[ 6],
		Elements[10],
		Elements[14],
		Elements[ 3],
		Elements[ 7],
		Elements[11],
		Elements[15]);
}

//----------------------------------------------------------------------------
template<typename Real>
void Matrix4<Real>::Transpose()
{
	std::swap(M12, M21);
	std::swap(M13, M31);
	std::swap(M14, M41);
	std::swap(M23, M32);
	std::swap(M24, M42);
	std::swap(M34, M43);

}

//----------------------------------------------------------------------------
template<typename Real>
Real Matrix4<Real>::Determinant() const
{
	Real fA0 = Elements[ 0]*Elements[ 5] - Elements[ 1]*Elements[ 4];
	Real fA1 = Elements[ 0]*Elements[ 6] - Elements[ 2]*Elements[ 4];
	Real fA2 = Elements[ 0]*Elements[ 7] - Elements[ 3]*Elements[ 4];
	Real fA3 = Elements[ 1]*Elements[ 6] - Elements[ 2]*Elements[ 5];
	Real fA4 = Elements[ 1]*Elements[ 7] - Elements[ 3]*Elements[ 5];
	Real fA5 = Elements[ 2]*Elements[ 7] - Elements[ 3]*Elements[ 6];
	Real fB0 = Elements[ 8]*Elements[13] - Elements[ 9]*Elements[12];
	Real fB1 = Elements[ 8]*Elements[14] - Elements[10]*Elements[12];
	Real fB2 = Elements[ 8]*Elements[15] - Elements[11]*Elements[12];
	Real fB3 = Elements[ 9]*Elements[14] - Elements[10]*Elements[13];
	Real fB4 = Elements[ 9]*Elements[15] - Elements[11]*Elements[13];
	Real fB5 = Elements[10]*Elements[15] - Elements[11]*Elements[14];
	Real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
	return fDet;
}

//----------------------------------------------------------------------------
template <typename Real>
inline Matrix4<Real> operator* (Real fScalar, const Matrix4<Real>& rhs)
{
	return rhs * fScalar;
}

//////////////////////////////////////////////////////////////////////////
// 重载模板函数，帮助编辑器找到正确的
//////////////////////////////////////////////////////////////////////////
template <typename Real>
inline Matrix4<Real> operator* (int fScalar, const Matrix4<Real>& rhs)
{
	return rhs * static_cast<Real>(fScalar);
}

template <typename Real>
inline Matrix4<Real> operator* (unsigned int fScalar, const Matrix4<Real>& rhs)
{
	return rhs * static_cast<Real>(fScalar);
}
//////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
template <typename Real>
inline Vector<Real, 4> operator* (const Vector<Real, 4>& lhs, const Matrix4<Real>& rhs)
{
	return Vector4<Real>(
		lhs[0]*rhs.M11+lhs[1]*rhs.M21+lhs[2]*rhs.M31+lhs[3]*rhs.M41,
		lhs[0]*rhs.M12+lhs[1]*rhs.M22+lhs[2]*rhs.M32+lhs[3]*rhs.M42,
		lhs[0]*rhs.M13+lhs[1]*rhs.M23+lhs[2]*rhs.M33+lhs[3]*rhs.M43,
		lhs[0]*rhs.M14+lhs[1]*rhs.M24+lhs[2]*rhs.M34+lhs[3]*rhs.M44);
}