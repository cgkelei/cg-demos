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
	Vector<Real, 3> difference = Center - sphere.Center;

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
	Vector<Real, 3> difference = Center - sphere.Center;

	Real distance = difference.Length();

	if( Radius + sphere.Radius < distance )
		return CT_Disjoint;

	if( Radius - sphere.Radius < distance )
		return CT_Intersects;

	return CT_Contains;
}

template<typename Real>
ContainmentType BoundingSphere<Real>::Contains( const BoundingBox<Real>& box )
{
	Vector<Real,3> vector;

	if( !box.Intersects(*this) )
		return CT_Disjoint;

	Real radiusSquared = Radius * Radius;

	vector.X() = Center.X() - box.Min.X();
	vector.Y() = Center.Y() - box.Max.Y();
	vector.Z() = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radiusSquared )
		return CT_Intersects;

	vector.X = Center.X() - box.Max.X();
	vector.Y = Center.Y() - box.Max.Y();
	vector.Z = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Max.X();
	vector.Y = Center.Y() - box.Min.Y();
	vector.Z = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Min.X();
	vector.Y = Center.Y() - box.Min.Y();
	vector.Z = Center.Z() - box.Max.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Min.X();
	vector.Y = Center.Y() - box.Max.Y();
	vector.Z = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Max.X();
	vector.Y = Center.Y() - box.Max.Y();
	vector.Z = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Max.X();
	vector.Y = Center.Y() - box.Min.Y();
	vector.Z = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radius )
		return CT_Intersects;

	vector.X = Center.X() - box.Min.X();
	vector.Y = Center.Y() - box.Min.Y();
	vector.Z = Center.Z() - box.Min.Z();

	if( vector.SquaredLength() > radius )
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



template<typename Real>
BoundingSphere<Real> FromBox( const BoundingBox<Real>& box )
{
	Vector<Real,3> difference = box.Max - box.Min;
	Vector<Real,3> center = (box.Min + box.Max) * Real(0.5);
	Real radius = difference.Length() * Real(0.5);

	return BoundingSphere<Real>(center, radius);
}

template<typename Real>
BoundingSphere<Real> Merge(const BoundingSphere<Real>& sphere1,const BoundingSphere<Real>& sphere2 )
{
	BoundingSphere sphere;
	Vector<Real,3> difference = sphere2.Center - sphere1.Center;

	Real length = difference.Length();
	Real radius = sphere1.Radius;
	Real radius2 = sphere2.Radius;

	if( radius + radius2 >= length)
	{
		if( radius - radius2 >= length )
			return sphere1;

		if( radius2 - radius >= length )
			return sphere2;
	}

	Vector<Real,3> vector = difference * ( Real(1) / length );
	Real min = (std::min)( -radius, length - radius2 );
	Real max = (std::max)( radius, length + radius2 ) - min ) * 0.5f;

	sphere.Center = sphere1.Center + vector * ( max + min );
	sphere.Radius = max;

	return sphere;
}


