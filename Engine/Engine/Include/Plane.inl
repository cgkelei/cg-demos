//  [8/23/2011 hustruan]
//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>::Plane3()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>::Plane3(const Plane3<Real>& rhs)	
	: Normal(rhs.Normal)
{
	Constant = rhs.Constant;
}

//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>::Plane3( const Vector<Real, 3>& rkNormal, Real fConstant )
	: Normal(rkNormal)
{
	Constant = fConstant; 
}

//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>::Plane3(const Vector<Real, 3>& rkNormal, const Vector<Real, 3>& rkP)
	:Normal(rkNormal)
{
	Constant = MathUtil::Dot(rkNormal, rkP);
}

//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>::Plane3(const Vector<Real, 3>& rkPoint1, const Vector<Real, 3>& rkPoint2, const Vector<Real, 3>& rkPoint3)
{
	Vector<Real, 3> kEdge1 = rkPoint2 - rkPoint1;
	Vector<Real, 3> kEdge2 = rkPoint3 - rkPoint1;
	Normal = MathUtil::Normalize(MathUtil::Cross(kEdge1, kEdge2));
	Constant = MathUtil::Dot(Normal, rkPoint1);
}

//----------------------------------------------------------------------------
template< typename Real >
Plane3<Real>& Plane3<Real>::operator=( const Plane3<Real>& rkPlane )
{
	Normal = rkPlane.Normal;
	Constant = rkPlane.Constant;
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real >
PlaneSide Plane3<Real>::WhichSide (const Vector<Real, 3>& rkPoint) const
{
	Real fDistance = DistanceTo(rkPoint);

	if (fDistance < (Real)0.0)
	{
		return Negative_Side;
	}

	if (fDistance > (Real)0.0)
	{
		return Positive_Side;
	}

	return No_Side;
}

//----------------------------------------------------------------------------
template< typename Real >
Real Plane3<Real>::DistanceTo(const Vector<Real, 3>& rkQ) const
{
	return MathUtil::Dot(Normal, rkQ) - Constant;
}

