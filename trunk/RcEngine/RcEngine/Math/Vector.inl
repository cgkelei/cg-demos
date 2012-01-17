//  [8/22/2011 hustruan]

//----------------------------------------------------------------------------
template <class Real, int Size>
Vector<Real, Size>::Vector()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real, int Size>
Vector<Real, Size>::Vector(Real fX, Real fY)
{
	mTuple[0] = fX;
	mTuple[1] = fY;
}

//----------------------------------------------------------------------------
template <class Real, int Size>
Vector<Real, Size>::Vector(Real fX, Real fY, Real fZ)
{
	mTuple[0] = fX;
	mTuple[1] = fY;
	mTuple[2] = fZ;
}

//----------------------------------------------------------------------------
template <class Real, int Size>
Vector<Real, Size>::Vector(Real fX, Real fY, Real fZ, Real fW)
{
	mTuple[0] = fX;
	mTuple[1] = fY;
	mTuple[2] = fZ;
	mTuple[3] = fW;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>::Vector( const Real* fTuple )
{
	for(int i = 0; i < Size; i++)
		mTuple[i] = fTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>::Vector( Real value )
{
	for(int i = 0; i < Size; i++)
		mTuple[i] = value;
}

//----------------------------------------------------------------------------
template< typename Real, int Size > template<typename T>
Vector<Real, Size>::Vector( const Vector<T, Size>& rhs )
{
	for(int i = 0; i < Size; i++)
		mTuple[i] = rhs[i];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>& Vector<Real, Size>::operator=( const Vector& rhs )
{
	for(int i = 0; i < Size; i++)
		mTuple[i] = rhs[i];
	return *this;
}


//----------------------------------------------------------------------------
template< typename Real, int Size >
const Real* Vector<Real, Size>::operator()() const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real* Vector<Real, Size>::operator()()
{
	return mTuple;
}


//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::operator[]( size_t i ) const
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real& Vector<Real, Size>::operator[]( size_t i )
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::X() const 
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real& Vector<Real, Size>::X()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::Y() const 
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real& Vector<Real, Size>::Y()
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::Z() const 
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real& Vector<Real, Size>::Z()
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::W() const 
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real& Vector<Real, Size>::W()
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template<typename Real, int Size>
Vector<Real, Size> Vector<Real, Size>::operator+( const Vector<Real, Size>& rhs ) const
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] = rhs[i] + mTuple[i];

	return result;
}

//----------------------------------------------------------------------------

template<typename Real, int Size>
Vector<Real, Size> Vector<Real, Size>::operator-( const Vector<Real, Size>& rhs ) const
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] = mTuple[i]- rhs[i];

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int Size>
Real Vector<Real, Size>::operator*( const Vector<Real, Size>& rhs) const
{
	Real result = (Real)0;
	for(int i = 0; i < Size; i++)
		result +=  mTuple[i] * rhs[i];

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int Size>
Vector<Real, Size> Vector<Real, Size>::operator*( Real fScalar ) const
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  mTuple[i] * fScalar;

	return result;
}

//----------------------------------------------------------------------------

template<typename Real, int Size>
Vector<Real, Size> Vector<Real, Size>::operator/( Real fScalar ) const
{
	Vector<Real, Size> result;

	Real fInvScalar = ((Real)1.0)/fScalar;

	for(int i = 0; i < Size; i++)
		result[i] = mTuple[i] * fInvScalar;

	return result;
}

//----------------------------------------------------------------------------
template<typename Real, int Size>
Vector<Real, Size> Vector<Real, Size>::operator-() const
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  -mTuple[i];

	return result;
}

//----------------------------------------------------------------------------

template< typename Real, int Size >
Vector<Real, Size>& Vector<Real, Size>::operator+=( const Vector<Real, Size>& rhs )
{
	for(int i = 0; i < Size; i++)
		 mTuple[i]  += rhs[i];
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>& Vector<Real, Size>::operator-=( const Vector<Real, Size>& rhs )
{
	for(int i = 0; i < Size; i++)
		mTuple[i]  -= rhs[i];
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>& Vector<Real, Size>::operator*=( Real fScalar)
{
	for(int i = 0; i < Size; i++)
		mTuple[i]  *= fScalar;
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Vector<Real, Size>& Vector<Real, Size>::operator/=( Real fScalar)
{
	Real fInvScalar = ((Real)1.0)/fScalar;

	for(int i = 0; i < Size; i++)
		mTuple[i]  *= fInvScalar;

	return *this;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
bool Vector<Real, Size>::operator==( const Vector<Real, Size>& rhs ) const
{
	 return CompareArrays(rhs) == 0;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
bool Vector<Real, Size>::operator!=( const Vector<Real, Size>& rhs ) const
{
	 return CompareArrays(rhs) != 0;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
int Vector<Real, Size>::CompareArrays( const Vector<Real, Size>& rhs ) const
{
	return memcmp(mTuple,rhs.mTuple,Size*sizeof(Real));
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::SquaredLength() const
{
	Real result = (Real)0.0;
	for(int i = 0; i < Size; i++)
		result +=  mTuple[i] * mTuple[i] ;
	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::Dot( const Vector<Real, Size>& rhs ) const
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  mTuple[i] * fScalar;

	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
void Vector<Real, Size>::Normalize()
{
	Real fLength = SquaredLength();
	if(fLength > (Real)0.0)
	{
		Real fInvScalar = ((Real)1.0)/ ((Real)sqrt((double)fLength));

		for(int i = 0; i < Size; i++)
			mTuple[i] *= fInvScalar;
	}
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
 Vector<Real, Size> Vector<Real, Size>::Zero() 
{
	static Vector<Real, Size> zero((Real)0);
	return zero;
}

//----------------------------------------------------------------------------
template <typename Real, int Size>
Vector<Real, Size> operator* (Real fScalar, const Vector<Real, Size>& rhs)
{
	Vector<Real, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  fScalar * rhs[i];

	return result;
}
//----------------------------------------------------------------------------
template <int Size>
Vector<float, Size> operator* (int fScalar, const Vector<float, Size>& rhs)
{
	Vector<float, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  fScalar * rhs[i];

	return result;
}
//----------------------------------------------------------------------------
template <int Size>
Vector<float, Size> operator* (unsigned int fScalar, const Vector<float, Size>& rhs)
{
	Vector<float, Size> result;
	for(int i = 0; i < Size; i++)
		result[i] =  fScalar * rhs[i];

	return result;
}

//----------------------------------------------------------------------------
template< typename Real, int Size >
Real Vector<Real, Size>::Length() const
{
	Real result = 0;
	for(int i = 0; i < Size; i++)
		result +=  mTuple[i] * mTuple[i] ;
	return (Real)sqrt((double)result);
}

//----------------------------------------------------------------------------
// debugging output
static const char* VectorMsg[] = { "X = ", ",Y = ", ",Z = ", ",W = " };

template <class Real, int Size>
std::ostream& operator<< (std::ostream& rkOStr, const Vector<Real, Size>& rhs)
{
	int i = 0;
	while( i < Size )
	{
		rkOStr << VectorMsg[i] << rhs[i] << " ";
		i++;
	}
	return rkOStr << std::endl;
}
