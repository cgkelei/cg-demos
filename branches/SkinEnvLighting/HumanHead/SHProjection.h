#ifndef SHProjection_h__
#define SHProjection_h__

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

template<typename Real>
class SHProjection;

// Forward declarations of some template friend operators of class SHProjection.
template<typename Real>
const SHProjection<Real> operator + ( const SHProjection< Real >& lhs, const SHProjection< Real >& rhs );

template<typename Real>
const SHProjection<Real> operator * ( const SHProjection< Real >& lhs, const SHProjection< Real >& rhs );

template<typename Real, typename Scalar>
const SHProjection<Real> operator * ( Scalar lhs, const SHProjection< Real >& rhs );

template<typename Real, typename Scalar>
const SHProjection<Real> operator * ( const SHProjection< Real >& lhs, Scalar rhs );

template<typename Real>
std::ostream& operator << ( std::ostream& os, const SHProjection<Real>& projection );

template <typename Real>
class SHProjection
{
public:

	static const Real PI;
	typedef std::vector<Real> SHCoefficients;

	/// Enumeration of available numerical integration methods
	enum IntegrationMethods
	{
		IM_MonteCarlo,            ///< plain Monte Carlo integration over the sphere's surface
		IM_MonteCarloJittered,    ///< Monte Carlo integration using jittered sampling
		IM_Simpson,               ///< integration using Simpson's rule
	};

public:

	template <class Function>
	explicit SHProjection( const Function& function, 
		int order = 2, 
		int sqrtNumSamples = 50, 
		IntegrationMethods integrationMethod = IM_MonteCarloJittered );

	/// Evaluates this projection at the spherical coordinates (theta, phi, 1).
	Real Evaluate( Real theta, Real phi ) const;

	/// Evaluates this projection at the Cartesian coordinates (x,y,z).
	Real Evaluate( Real x, Real y, Real z ) const;

	/// Returns the number of bands used for this projection (ie the order of approximation).
	const int GetNumBands() const { return mOrder + 1; }

	/// Returns the SH coefficients of this projection.
	const SHCoefficients& GetProjection() const { return mCoefficients; }

	/// Returns the SH coefficient belonging to Y(l,m).
	const Real GetCoefficient( int l, int m ) const { return mCoefficients[l*(l+1) + m]; }

	/// Returns the i-th SH coefficient.
	const Real GetCoefficient( int i ) const { return mCoefficients[i]; }

	/// Const access to the i-th SH coefficient.
	const Real& operator [] ( int i ) const { return mCoefficients[i]; }

	/// Non-const access to the i-th SH coefficient.
	Real& operator [] ( int i ) { return mCoefficients[i]; }

	/// Adds another SHProjection to this one (component-wise).
	/// If other has a higher order this SHProjection's order is increased.
	/// This is equivalent to a addition of the represented functions.
	const SHProjection& operator += ( const SHProjection& other );

	/// Multiplies this SHProjection by another one (component-wise).
	/// If other has a higher order this SHProjection's order is increased.
	const SHProjection& operator *= ( const SHProjection& other );

	/// Multiplies this SHProjection by a constant factor (component-wise).
	/// This is equivalent to a scaling of the represented function.
	const SHProjection& operator *= ( Real factor );

	/// Performs component-wise addition of two SHProjections.
	/// The order of the result is the maximum order of the arguments.
	/// This is equivalent to an addition of the represented functions.
	friend const SHProjection operator + <> ( const SHProjection& lhs, const SHProjection& rhs );

	/// Performs component-wise multiplication of two SHProjections.
	/// The order of the result is the maximum order of the arguments.
	friend const SHProjection operator * <> ( const SHProjection& lhs, const SHProjection& rhs );

	/// Multiplies a SHProjection by a constant factor.
	/// This is equivalent to a scaling of the represented function.
	template< typename Real, typename Scalar>
	friend const SHProjection operator * <> ( Scalar lhs, const SHProjection& rhs );

	/// Multiplies a SHProjection by a constant factor.
	/// This is equivalent to a scaling of the represented function.
	template< typename Real, typename Scalar>
	friend const SHProjection operator * <> ( const SHProjection& lhs, Scalar rhs );

	/// Writes an SHProjection to an ostream.
	friend std::ostream& operator << <> ( std::ostream& os, const SHProjection& projection );

public:
	static void SphericalToCartesian(Real* cart, const Real* spherical);
	static void CartesianToSpherical(Real* spherical, const Real* cartesian);

	/// Get number of bands.
	static int NumBand(int order)	{ return order + 1; }

	/// Get number of sh basis.
	static int NumBasis(int order) { return (order + 1) * (order + 1); }

	/// Computes the inner product of two SHProjections (aka the dot product).
	/// This is equivalent to the integral of the product of the represented functions.
	static Real InnerProduct( const SHProjection& lhs, const SHProjection& rhs );

	/// Simple incremental factorial helper function.
	static int Factorial(int n);

	/// Evaluate an Associated Legendre Polynomial P(l, m) at x
	static Real P(int l, int m, Real x);

	/// Calculate the normalisation constant for an SH function
	/// No need to use |m| since SH always passes positive m
	static Real K(int l, int m);

	/// Evaluates the real SH basis function with parameters (l,m) at (theta,phi).
	static Real Y( int l, int m, Real theta, Real phi );

	/// Simple helper to get a sequence of independent uniformly distributed pseudo-random variables in [0,1).
	static Real RandFloat();

private:

	/// Projects function using plain Monte Carlo integration.
	template <class Function>
	void ProjectMonteCarlo( const Function& function, int numBands, int numSamples );

	/// Projects function using Jittered Monte Carlo integration.
	template < class Function >
	void ProjectMonteCarloJittered( const Function& function, int numBands, int numSamples );

	/// Projects function using Simpson's rule.
	template < class Function >
	void ProjectSimpson( const Function& function, int numBands, int numSamples );

private:
	int mOrder;
	SHCoefficients mCoefficients;
};

template<typename Real>
void SHEvalDirection(Real* pOut, int order, Real* dir);

#include "SHProjection.inl"

#endif // SHProjection_h__