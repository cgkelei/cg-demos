template<typename Real>
void BoundingSphere<Real>::Merge( const Vector<Real, 3>& point )
{
	if (!Defined)
	{
		Center = point;
		Radius = Real(0);
		Defined = true;
	}
	else
	{
		Vector<Real, 3> offset = point - Center;
		Real dist = offset.Length();

		if (dist > Radius)
		{
			Real half = (dist - Radius) / 2;
			Radius += half;
			Center += (half / dist) * offset;
		}
	}
}

template<typename Real>
void BoundingSphere<Real>::Merge( const BoundingSphere<Real>& sphere )
{
	BoundingSphere<Real> result;
	Vector<Real, 3> difference = Center() - sphere.Center();

	Real length = difference.Length();
	Real radius1 = Radius;
	Real radius2 = sphere.Radius;

	if (radius1 + radius2 > length)
	{
		if( radius1 - radius2 >= length )
			return *this;

		if( radius2 - radius1 >= length )
			return sphere;
	}

	Vector<Real, 3> vector = difference * ( Real(1) / length );
	float min = (std::min)( -radius1, length - radius2 );
	float max = (std::max)( radius1, length + radius2 ) - min ) * Real(0.5);

	result.Center = Center + vector * ( max + min );
	result.Radius = max;

	return result;
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const BoundingSphere<Real>& sphere )
{
	Real distance;
	Real x = Center.X() - sphere.Center.X();
	Real y = Center.Y() - sphere.Center.Y();
	Real z = Center.Z() - sphere.Center.Z();

	distance = std::sqrt( (x * x) + (y * y) + (z * z) );

	if( Radius + sphere.Radius < distance )
		return CT_Disjoint;

	if( Radius - sphere.Radius < distance )
		return CT_Intersects;

	return CT_Contains;
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const Vector<Real,3>& vector )
{
	Vector<Real, 3> offset = vector - Center;
	Real distanceSquared = offset.SquaredLength();

	if( distanceSquared >= (Radius * Radius) )
		return CT_Disjoint;

	return CT_Contains;
}

template<typename Real>
bool BoundingSphere<Real>::Intersects( const BoundingSphere<Real>& sphere )
{
	Real distanceSquared = (Center - sphere.Center).SquaredLength();
	Real conbine = this->Radius + sphere.Radius;

	if (conbine * conbine <= distanceSquared)
		return false;

	return true;
}

template<typename Real>
bool BoundingSphere<Real>::Intersects( const BoundingBox<Real>& box )
{
	Vector<Real, 3> vector;

	if( !Intersects( box, *this ) )
		return CT_Disjoint;

	Real radiusSquared = Radius * Radius;
	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Max.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Min.Y();
	vector.Z() = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	return CT_Contains;
}
