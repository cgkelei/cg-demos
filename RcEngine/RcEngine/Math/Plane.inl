//  [8/23/2011 hustruan]
//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Plane<Real>& rhs)	
	: Normal(rhs.Normal)
{
	Distance = rhs.Distance;
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane( const Vector<Real, 3>& rkNormal, Real fDistance )
	: Normal(rkNormal)
{
	Distance = fDistance; 
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Vector<Real, 3>& rkNormal, const Vector<Real, 3>& rkP)
	:Normal(rkNormal)
{
	Distance = -Dot(rkNormal, rkP);
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>::Plane(const Vector<Real, 3>& rkPoint1, const Vector<Real, 3>& rkPoint2, const Vector<Real, 3>& rkPoint3)
{
	Vector<Real, 3> kEdge1 = rkPoint2 - rkPoint1;
	Vector<Real, 3> kEdge2 = rkPoint3 - rkPoint1;
	Normal = Normalize(Cross(kEdge1, kEdge2));
	Distance = -Dot(Normal, rkPoint1);
}

//----------------------------------------------------------------------------
template< typename Real >
Plane<Real>& Plane<Real>::operator=( const Plane<Real>& rkPlane )
{
	Normal = rkPlane.Normal;
	Distance = rkPlane.Distance;
	return *this;
}

//----------------------------------------------------------------------------
template< typename Real >
PlaneSide Plane<Real>::WhichSide (const Vector<Real, 3>& rkPoint) const
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
Real Plane<Real>::DistanceTo(const Vector<Real, 3>& rkQ) const
{
	return Dot(Normal, rkQ) + Distance;
}

template< typename Real >
void Plane<Real>::Normalize( Real epsilon /*= Math<Real>::ZERO_TOLERANCE*/ )
{
	Real length = Normal.Length();

	if (length > epsilon)
	{
		Real invLength = Real(0) / length;
		Normal *= invLength;
		Distance *= invLength;
	}
}

