template <typename Real>
template <class Function>
inline SHProjection<Real>::SHProjection( const Function& function, int order /*= 2*/, int sqrtNumSamples /*= 50*/, IntegrationMethods integrationMethod /*= IM_Simpson */ )
	: mOrder(order)
{
	if( integrationMethod == IM_MonteCarloJittered )
		ProjectMonteCarloJittered( function, NumBand(order), sqrtNumSamples );
	else if( integrationMethod == IM_Simpson )
		ProjectSimpson( function, NumBand(order), sqrtNumSamples );
	else
		ProjectMonteCarlo( function, NumBand(order), sqrtNumSamples );
}

template <typename Real>
Real SHProjection<Real>::Evaluate( Real x, Real y, Real z ) const
{
	Real cart[3] = {x,y,z};
    Real sphe[3];

    CartesianToSpherical(sphe,cart);
    return Evaluate(sphe[1],sphe[0]);
}

template <typename Real>
Real SHProjection<Real>::Evaluate( Real theta, Real phi ) const
{
	Real result = Real(0);

	for( int l=0; l<mNumBands; ++l )
		for( int m=-l; m<=l; ++m )
			result += GetCoefficient(l,m) * Y(l,m,theta,phi);

	return result;
}

template <typename Real>
inline const SHProjection<Real>& SHProjection<Real>::operator+=( const SHProjection& other )
{
	assert(other.mOrder == mOrder);

	for( std::size_t i = 0; i < mCoefficients.size(); ++i )
		mCoefficients[i] += other.mCoefficients[i];

	return *this;
}

template< typename Real >
inline const SHProjection<Real>& SHProjection<Real>::operator *= ( const SHProjection& other )
{
	assert(other.mOrder == mOrder);

	for( std::size_t i = 0; i < mCoefficients.size(); ++i )
		mCoefficients[i] *= other.mCoefficients[i];

	return *this;
}

template <typename Real>
inline const SHProjection<Real>& SHProjection<Real>::operator *= ( Real factor )
{
	const std::size_t n = mCoefficients.size();
	for( std::size_t i=0; i<n; ++i )
		mCoefficients[i] *= factor;

	return *this;
}


template <typename Real>
inline const SHProjection<Real> operator+( const SHProjection<Real>& lhs, const SHProjection<Real>& rhs )
{
	SHProjection<Real> ret( lhs );
	ret += rhs;
	return ret;
}


template< typename Real>
inline const SHProjection< Real > operator * ( const SHProjection<Real>& lhs, const SHProjection<Real>& rhs )
{
	SHProjection<Real> ret( lhs );
	ret *= rhs;
	return ret;
}

template< typename Real, typename Scalar>
inline const SHProjection<Real> operator * ( Scalar lhs, const SHProjection<Real>& rhs )
{
	SHProjection<Real> ret( rhs );
	ret *= static_cast<Real>(lhs);
	return ret;
}

template< typename Real, typename Scalar>
inline const SHProjection<Real> operator * ( const SHProjection<Real>& lhs, Scalar rhs )
{
	SHProjection<Real> ret( lhs );
	ret *= static_cast<Real>(rhs);
	return ret;
}


template <typename Real>
inline Real SHProjection<Real>::InnerProduct( const SHProjection<Real>& lhs, const SHProjection<Real>& rhs )
{
	const std::size_t n = std::min( lhs.mCoefficients.size(), rhs.mCoefficients.size() );
	return std::inner_product(
		lhs.mCoefficients.begin(), lhs.mCoefficients.begin() + n,
		rhs.mCoefficients.begin(),
		Real() );
}

template <typename Real>
inline Real SHProjection<Real>::K( int l, int m )
{
	return sqrt( ( (2*l+1) * Factorial(l-m) ) / (4 * PI * Factorial(l+m)));
}


template <typename Real>
inline Real SHProjection<Real>::P( int l, int m, Real x )
{
	//First generate the value of P(m, m) at x
	Real pmm = 1;
	if( m > 0 )
	{
		Real somx2 = sqrt( (1-x)*(1+x) );
		Real fact = 1;
		for( int i=1; i<=m; ++i )
		{
			pmm *= -fact * somx2;
			fact += 2;
		}
	}

	if( l == m )
		return pmm;

	Real pmmp1 = x * (2*m+1) * pmm;
	if( l == m+1 )
		return pmmp1;

	Real pll = 0.0;
	for( int ll=m+2; ll<=l; ++ll )
	{
		pll = ( (2*ll-1) * x * pmmp1 - (ll+m-1) * pmm ) / (ll-m);
		pmm = pmmp1;
		pmmp1 = pll;
	}

	return pll;
}


template <typename Real>
int SHProjection<Real>::Factorial( int n )
{
	int result = 1;

	for( int i=2; i<=n; ++i )
		result *= i;

	return result;
}


template <typename Real>
inline Real SHProjection<Real>::Y( int l, int m, Real theta, Real phi )
{
	const Real sqrt2=sqrt(Real(2));

	if(m==0)
		return K(l, 0)*P(l, m, cos(theta));

	if(m>0)
		return sqrt2*K(l, m)*cos(m*phi)*P(l, m, cos(theta));

	//m<0
	return sqrt2*K(l,-m)*sin(-m*phi)*P(l, -m, cos(theta));
}


template <typename Real>
inline Real SHProjection<Real>::RandFloat()
{
	return static_cast<Real>( rand() ) / RAND_MAX;
}

template <typename Real>
template < class Function >
void SHProjection<Real>::ProjectSimpson( const Function& function, int numBands, int sqrtNumSamples )
{

}

template <typename Real>
template < class Function >
void SHProjection<Real>::ProjectMonteCarloJittered( const Function& function, int numBands, int sqrtNumSamples )
{
	const int numSamples=sqrtNumSamples*sqrtNumSamples;
	const int numCoeffs = numBands*numBands;
	const Real oneOverN = 1 / (Real)sqrtNumSamples;

	// prepare the coefficients vector
	mCoefficients.clear();
	mCoefficients.resize( numCoeffs, 0 );

	for(int i=0; i<sqrtNumSamples; ++i)
	{
		for(int j=0; j<sqrtNumSamples; ++j)
		{
			Real x = (i + RandFloat()) * oneOverN;
			Real y = (j + RandFloat()) * oneOverN;

			Real theta = 2 * acos( sqrt(Real(1) - x) );
			Real phi = 2 * PI * y;

			for (int l = 0; l < numBands; ++l)
			{
				for (int m = -l; m <= l; ++m)
				{
					int index = l*(l+1) + m;
					mCoefficients[index] += Y(l,m,theta,phi) * function(theta, phi); 
				}
			}
		}
	}

	// divide by the number of samples and multiply by the area
	// of the integration region to get the final result
	const Real normalizationFactor = 4 * PI / numSamples;
	for( int k=0; k<numCoeffs; ++k )
		mCoefficients[k] = mCoefficients[k] * normalizationFactor;
}

template <typename Real>
template <class Function>
void SHProjection<Real>::ProjectMonteCarlo( const Function& function, int numBands, int sqrtNumSamples )
{
	const int numSamples=sqrtNumSamples*sqrtNumSamples;
	const int numCoeffs = numBands*numBands;

	// prepare the coefficients vector
	mCoefficients.clear();
	mCoefficients.resize( numCoeffs, 0 );

	for (int i = 0; i < numSamples; ++i)
	{
		Real x = RandFloat();
		Real y = RandFloat();

		Real theta = 2 * acos( sqrt(Real(1) - x) );
		Real phi = 2 * PI * y;

		for (int l = 0; l < numBands; ++l)
		{
			for (int m = -l; m <= l; ++m)
			{
				int index = l*(l+1) + m;
				mCoefficients[index] += Y(l,m,theta,phi) * function(theta, phi); 
			}
		}
	}

	// divide by the number of samples and multiply by the area
	// of the integration region to get the final result
	const Real normalizationFactor = 4 * PI / numSamples;
	for( int k=0; k<numCoeffs; ++k )
		mCoefficients[k] = mCoefficients[k] * normalizationFactor;
}


template< typename Real >
std::ostream& operator << ( std::ostream& os, const SHProjection<Real>& projection )
{
	const SHProjection< Real >::SHCoefficients& coeffs = projection.mCoefficients;
	const std::size_t n = coeffs.size();

	const int numBands = projection.GetNumBands();
	for( int l=0; l < numBands; ++l )
		for( int m=-l; m<=l; ++m )
			os << projection.GetCoefficient(l, m) << ( m == l ? "\n" : ", " );

	//return ss.str();

	//os << "[";
	//for( std::size_t i=0; i<n; ++i )
	//{
	//	os <<GetCoefficient(l, m) << ( m == l ? "\n" : ", " );

	//	//os << coeffs[i] << ( i == n-1 ? "" : ", " );
	//}
	//os << "]";

	return os;
}

template <typename Real>
void SHProjection<Real>::SphericalToCartesian( Real* cart, const Real* spherical )
{
	Real theta = spherical[0];
	Real phi = spherical[1];
	Real r = spherical[2];
	cart[0] = (Real)(r * cos(phi) * sin(theta));
	cart[1] = (Real)(r * sin(phi) * sin(theta));
	cart[2] = (Real)(r * cos(theta));
}

template <typename Real>
void SHProjection<Real>::CartesianToSpherical( Real* spherical, const Real* cartesian )
{
	Real x = (Real)(cartesian[0]);
	Real y = (Real)(cartesian[1]);
	Real z = (Real)(cartesian[2]);

	Real r = sqrt(x * x + y * y + z * z);
	Real theta = acos(z / r);
	Real phi = atan2(y, x);
	phi = phi < 0 ? 2 * PI + phi : phi;

	spherical[0] = theta;
	spherical[1] = phi;
	spherical[2] = r;
}

template<typename Real>
void SHEvalDirection( Real* pOut, int order, Real* dir )
{
	const int numBand = order + 1;
	
	Real spherical[3]; 
	SHProjection<Real>::CartesianToSpherical(dir);

	for( int l=0; l < numBands; ++l )
	{
		for( int m=-l; m<=l; ++m )
		{
			int index = l*(l+1) + m;
			pOut[index] = SHProjection<Real>::Y(l, m, spherical[0], spherical[1]);
		}
	}
}


