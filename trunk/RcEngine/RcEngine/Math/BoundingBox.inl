template<typename Real>
void BoundingBox<Real>::Merge( const Vector<Real,3>& point )
{
	if (!Defined)
	{
		Max = Min = point;
		Defined = true;
		return;
	}

	if (point.X() < Min.X())	Min.X() = point.X();
	if (point.Y() < Min.Y())	Min.Y() = point.Y();
	if (point.Z() < Min.Z())	Min.X() = point.Z();

	if (point.X() > Max.X())	Max.X() = point.X();
	if (point.Y() > Max.Y())	Max.Y() = point.Y();
	if (point.Z() > Max.Z())	Max.X() = point.Z();

}

template<typename Real>
void BoundingBox<Real>::Merge( const BoundingBox<Real>& box )
{
	if (!Defined)
	{
		Min = box.Min;
		Max = box.Max;
		Defined = true;
		return;
	}

	if (box.Min.X() < Min.X())	Min.X() = box.Min.X();
	if (box.Min.Y() < Min.Y())	Min.Y() = box.Min.Y();
	if (box.Min.Z() < Min.Z())	Min.X() = box.Min.Z();

	if (box.Max.X() > Max.X())	Max.X() = box.Max.X();
	if (box.Max.Y() > Max.Y())	Max.Y() = box.Max.Y();
	if (box.Max.Z() > Max.Z())	Max.X() = box.Max.Z();
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const BoundingBox<Real>& box )
{
	if( Max.X() < box.Min.X() || Min.X() > box.Max.X() )
		return CT_Disjoint;

	if( Max.Y() < box.Min.Y() || Min.Y() > box.Max.Y() )
		return CT_Disjoint;

	if( Max.Z() < box.Min.Z() || Min.Z() > box.Max.Z() )
		return CT_Disjoint;

	if( Min.X() <= box.Min.X() && box.Max.X() <= Max.X() && 
		Min.Y() <= box.Min.Y() && box.Max.Y() <= Max.Y() &&
		Min.Z() <= box.Min.Z() && box.Max.Z() <= Max.Z() )
		return CT_Contains;

	return CT_Intersects;
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const BoundingSphere<Real>& sphere )
{
	Real distanceSquared, temp;
	const Vector<Real, 3>& center = sphere.Center;
	Real radius = sphere.Radius;

	if (center.X() < Min.X())
	{
		temp = center.X() - Min.X();
		distanceSquared += temp * temp;
	}
	else if (center.X() > Max.X())
	{
		temp = center.X() - Max.X();
		distanceSquared += temp * temp;
	}

	if (center.Y() < Min.Y())
	{
		temp = center.Y() - Min.Y();
		distanceSquared += temp * temp;
	}
	else if (center.Y() > Max.Y())
	{
		temp = center.Y() - Max.Y();
		distanceSquared += temp * temp;
	}

	if (center.Z() < Min.Z())
	{
		temp = center.Z() - Min.Z();
		distanceSquared += temp * temp;
	}
	else if (center.Z() > Max.Z())
	{
		temp = center.Z() - Max.Z();
		distanceSquared += temp * temp;
	}

	if( distanceSquared > (radius * radius) )
		return CT_Disjoint;

	if ( center.X() - radius >= Min.X() && center.X() + radius <= Max.X() && Max.X() - Min.X() > radius &&
		center.Y() - radius >= Min.Y() && center.Y() + radius <= Max.Y() && Max.Y() - Min.Y() > radius &&
		center.Z() - radius >= Min.Z() && center.Z() + radius <= Max.Z() && Max.Z() - Min.Z() > radius )
		return CT_Contains;

	return CT_Intersects;
}

template<typename Real>
ContainmentType BoundingBox<Real>::Contains( const Vector<Real,3>& vector )
{
	if( Min.X() <= vector.X() && vector.X() <= Max.X() && Min.Y() <= vector.Y() && 
		vector.Y() <= Max.Y() && Min.Z() <= vector.Z() && vector.Z() <= Max.Z() )
		return CT_Contains;

	return CT_Disjoint;
}

template<typename Real>
bool BoundingBox<Real>::Intersects( const BoundingSphere<Real>& sphere )
{
	Real distanceSquared, temp;
	const Vector<Real, 3>& center = sphere.Center;

	if (center.X() < Min.X())
	{
		temp = center.X() - Min.X();
		distanceSquared += temp * temp;
	}
	else if (center.X() > Max.X())
	{
		temp = center.X() - Max.X();
		distanceSquared += temp * temp;
	}

	if (center.Y() < Min.Y())
	{
		temp = center.Y() - Min.Y();
		distanceSquared += temp * temp;
	}
	else if (center.Y() > Max.Y())
	{
		temp = center.Y() - Max.Y();
		distanceSquared += temp * temp;
	}

	if (center.Z() < Min.Z())
	{
		temp = center.Z() - Min.Z();
		distanceSquared += temp * temp;
	}
	else if (center.Z() > Max.Z())
	{
		temp = center.Z() - Max.Z();
		distanceSquared += temp * temp;
	}

	return ( distanceSquared <= (sphere.Radius * sphere.Radius) );
}

template<typename Real>
bool BoundingBox<Real>::Intersects( const BoundingBox<Real>& box )
{
	if ( Max.X() < box.Min.X() || Min.X() > box.Max.X() )
		return false;

	if ( Max.Y() < box.Min.Y() || Min.Y() > box.Max.Y() )
		return false;

	return ( Max.Z() >= box.Min.Z() && Min.Z() <= box.Max.Z() );
}


template<typename Real>
BoundingBox<Real> FromSphere( const BoundingSphere<Real>& sphere )
{
	BoundingBox box;
	box.Min = Vector<Real, 3>( sphere.Center.X() - sphere.Radius, sphere.Center.Y() - sphere.Radius, sphere.Center.Z() - sphere.Radius );
	box.Max = Vector<Real, 3>( sphere.Center.X() + sphere.Radius, sphere.Center.Y() + sphere.Radius, sphere.Center.Z() + sphere.Radius );
	return box;
}

template<typename Real>
BoundingBox<Real> Merge( const BoundingBox<Real>& box1, const BoundingBox<Real>& box2 )
{
	Vector<Real,3> min, max;

	if (box1.Min.X() < box2.Min.X())	min.X() = box.Min.X();
	if (box1.Min.Y() < box2.Min.Y())	min.Y() = box.Min.Y();
	if (box1.Min.Z() < box2.Min.Z())	min.X() = box.Min.Z();

	if (box1.Max.X() > box2.Max.X())	max.X() = box.Max.X();
	if (box1.Max.Y() > box2.Max.Y())	max.Y() = box.Max.Y();
	if (box1.Max.Z() > box2.Max.Z())	max.X() = box.Max.Z();

	return BoundingBox<Real>(min, max);
}