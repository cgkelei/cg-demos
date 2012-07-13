//-------------------------------------------------------------------------
template<typename Real>
Quaternion<Real>::Quaternion()
{

}

template<typename Real>
Quaternion<Real>::Quaternion( Real w, Real x, Real y, Real z )
{
	mTuple[0] = w;
	mTuple[1] = x;
	mTuple[2] = y;
	mTuple[3] = z;
}

template<typename Real>
Quaternion<Real>::Quaternion( const Quaternion<Real>& rhs )
{
	mTuple[0] = rhs.mTuple[0];
	mTuple[1] = rhs.mTuple[1];
	mTuple[2] = rhs.mTuple[2];
	mTuple[3] = rhs.mTuple[3];
}

template<typename Real>
Real Quaternion<Real>::operator[]( int i ) const
{
	return mTuple[i];
}

template<typename Real>
Real& Quaternion<Real>::operator[]( int i )
{
	return mTuple[i];
}

template<typename Real>
Real Quaternion<Real>::W() const
{
	return mTuple[0];
}

template<typename Real>
Real& Quaternion<Real>::W()
{
	return mTuple[0];
}

template<typename Real>
Real Quaternion<Real>::X() const
{
	return mTuple[1];
}

template<typename Real>
Real& Quaternion<Real>::X()
{
	return mTuple[1];
}

template<typename Real>
Real Quaternion<Real>::Y() const
{
	return mTuple[2];
}

template<typename Real>
Real& Quaternion<Real>::Y()
{
	return mTuple[2];
}

template<typename Real>
Real Quaternion<Real>::Z() const
{
	return mTuple[3];
}

template<typename Real>
Real& Quaternion<Real>::Z()
{
	return mTuple[3];
}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator+=( const Quaternion<Real>& rhs )
{

}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator-=( const Quaternion<Real>& rhs )
{

}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator*=( const Quaternion<Real>& rhs )
{

}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator*=( float scalar )
{

}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator/=( float scalar )
{

}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator+( const Quaternion<Real>& rhs ) const
{

}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator-( const Quaternion<Real>& rhs ) const
{

}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator*( const Quaternion<Real>& rhs ) const
{

}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator*( float scalar ) const
{

}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator/( float scalar ) const
{

}

template<typename Real>
bool Quaternion<Real>::operator!=( const Quaternion<Real>& rhs ) const
{
	return CompareArrays(rhs) == 0;
}

template<typename Real>
bool Quaternion<Real>::operator==( const Quaternion<Real>& rhs ) const
{
	return CompareArrays(rhs) != 0;
}

template<typename Real>
int Quaternion<Real>::CompareArrays( const Quaternion<Real>& rhs ) const
{
	return memcmp(mTuple,rhs.mTuple,4*sizeof(Real));
}
