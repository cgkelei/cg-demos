
template<typename Real>
BoundingSphere<Real>::BoundingSphere()
{
	
}

template<typename Real>
BoundingSphere<Real>::BoundingSphere( const Vector<Real, 3>& center, Real radius )
	: mCenter(center), mRadius(radius)
{

}

template<typename Real>
Vector<Real, 3> BoundingSphere<Real>::Center() const
{
	return mCenter;
}

template<typename Real>
Vector<Real, 3>& BoundingSphere<Real>::Center()
{
	return mCenter;
}

template<typename Real>
Real BoundingSphere<Real>::Radius() const
{
	return mRadius;
}

template<typename Real>
Real& BoundingSphere<Real>::Radius()
{
	return mRadius;
}

