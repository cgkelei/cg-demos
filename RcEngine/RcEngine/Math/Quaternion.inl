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
void Quaternion<Real>::MakeIdentity()
{
	mTuple[0] = Real(1);
	mTuple[1] = Real(0);
	mTuple[2] = Real(0);
	mTuple[3] = Real(0);
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
	for (int i = 0; i < 4; i++)
	{
		mTuple[i] += rhs.mTuple[i];
	}
	return *this;
}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator-=( const Quaternion<Real>& rhs )
{
	for (int i = 0; i < 4; i++)
	{
		mTuple[i] -= rhs.mTuple[i];
	}
	return *this;
}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator*=( const Quaternion<Real>& rhs )
{
	*this = (*this) * rhs;
	return *this;
}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator*=( float scalar )
{
	for (int i = 0; i < 4; i++)
	{
		mTuple[i] *= scalar;
	}
	return *this;
}

template<typename Real>
Quaternion<Real>& Quaternion<Real>::operator/=( float scalar )
{
	int i;
	if (scalar != (Real)0.0)
	{
		Real invScalar = ((Real)1.0)/scalar;
		for (i = 0; i < 4; i++)
		{
			mTuple[i] *= invScalar;
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			mTuple[i] = (std::numeric_limits<Real>::max)();
		}
	}
}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator+( const Quaternion<Real>& rhs ) const
{
	Quaternion<Real> sum;
	for (int i = 0; i < 4; i++)
	{
		sum.mTuple[i] = mTuple[i] + rhs.mTuple[i];
	}
	return sum;
}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator-( const Quaternion<Real>& rhs ) const
{
	Quaternion<Real> diff;
	for (int i = 0; i < 4; i++)
	{
		diff.mTuple[i] = mTuple[i] - rhs.mTuple[i];
	}
	return diff;
}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator*( const Quaternion<Real>& rhs ) const
{
/*
 w = w1*w2 - x1*x2 - y1*y2 - z1*z2
 x = w1*x2 + x1*w2 + z1*y2 - y1*z2
 y = w1*y2 + y1*w2 + x1*z2 - z1*x2
 z = w1*z2 + z1*w2 + y1*x2 - x1*y2
 */
	return Quaternion<Real>(
		mTuple[0]*rhs.mTuple[0] - mTuple[1]*rhs.mTuple[1] - mTuple[2]*rhs.mTuple[2] - mTuple[3]*rhs.mTuple[3],
		mTuple[0]*rhs.mTuple[1] + mTuple[1]*rhs.mTuple[0] + mTuple[3]*rhs.mTuple[2] - mTuple[2]*rhs.mTuple[3],
		mTuple[0]*rhs.mTuple[2] + mTuple[2]*rhs.mTuple[0] + mTuple[1]*rhs.mTuple[3] - mTuple[3]*rhs.mTuple[1],
		mTuple[0]*rhs.mTuple[3] + mTuple[3]*rhs.mTuple[0] + mTuple[2]*rhs.mTuple[1] - mTuple[1]*rhs.mTuple[2] );
}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator*( float scalar ) const
{
	Quaternion<Real> prod;
	for (int i = 0; i < 4; i++)
	{
		prod.mTuple[i] = scalar*mTuple[i];
	}
	return prod;
}

template<typename Real>
Quaternion<Real> Quaternion<Real>::operator/( float scalar ) const
{
	Quaternion quot;
	int i;

	if (scalar != (Real)0.0)
	{
		Real invScalar = ((Real)1.0)/scalar;
		for (i = 0; i < 4; i++)
		{
			quot.mTuple[i] = invScalar*mTuple[i];
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			quot.mTuple[i] = (std::numeric_limits<Real>::max)();
		}
	}

	return quot;
}

template<typename Real>
bool Quaternion<Real>::operator!=( const Quaternion<Real>& rhs ) const
{
	return CompareArrays(rhs) != 0;
}

template<typename Real>
bool Quaternion<Real>::operator==( const Quaternion<Real>& rhs ) const
{
	return CompareArrays(rhs) == 0;
}

template<typename Real>
int Quaternion<Real>::CompareArrays( const Quaternion<Real>& rhs ) const
{
	return memcmp(mTuple,rhs.mTuple,4*sizeof(Real));
}
