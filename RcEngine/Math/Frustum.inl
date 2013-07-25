//////////////////////////////////////////////////////////////////////////
template<typename Real>
Frustum<Real>::Frustum()
{

}


//////////////////////////////////////////////////////////////////////////
template<typename Real>
void Frustum<Real>::Update( const Matrix4<Real>& viewProjection )
{
	// Left plane
	Planes[FPS_Left].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M11, 
											  viewProjection.M24 + viewProjection.M21, 
											  viewProjection.M34 + viewProjection.M31);

	Planes[FPS_Left].Distance = viewProjection.M44 + viewProjection.M41;
	

	// Right plane
	Planes[FPS_Right].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M11, 
											   viewProjection.M24 - viewProjection.M21, 
											   viewProjection.M34 - viewProjection.M31);

	Planes[FPS_Right].Distance= viewProjection.M44 - viewProjection.M41;

	// Top plane
	Planes[FPS_Top].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M12, 
											 viewProjection.M24 - viewProjection.M22, 
											 viewProjection.M34 - viewProjection.M32);

	Planes[FPS_Top].Distance = viewProjection.M44 - viewProjection.M42;


	// Bottom plane
	Planes[FPS_Bottom].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M12, 
												viewProjection.M24 + viewProjection.M22, 
												viewProjection.M34 + viewProjection.M32);

	Planes[FPS_Bottom].Distance = viewProjection.M44 + viewProjection.M42;

	// Near plane
	Planes[FPS_Near].Normal = Vector<Real, 3>(viewProjection.M13, viewProjection.M23, viewProjection.M33);
	Planes[FPS_Near].Distance = viewProjection.M43;


	// Far plane
	Planes[FPS_Far].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M13,
											 viewProjection.M24 - viewProjection.M23,
											 viewProjection.M34 - viewProjection.M33);
	Planes[FPS_Far].Distance = viewProjection.M44 - viewProjection.M43;

	for (int i = 0; i < 6; i++)		
		Planes[i].Normalize();
}

template<typename Real>
ContainmentType Frustum<Real>::Contain( const BoundingSphere<Real>& sphere )
{
	bool allInside = true;
	for (int32_t i=0; i < 6; ++i)
	{
		Real d = Planes[i].DistanceTo(sphere.Center);
		if (d <= -sphere.Radius)
		{
			return CT_Disjoint;
		}
		else if (d < sphere.Radius)
		{
			allInside = false;
			break;;
		}
	}

	return allInside ? CT_Contains : CT_Intersects;
}

template<typename Real>
ContainmentType Frustum<Real>::Contain( const BoundingBox<Real>& box )
{
	Vector<Real,3> center = box.Center();
	Vector<Real,3> half = center - box.Min;
	bool allInside = true;

	for (unsigned i = 0; i < 6; ++i)
	{
		Vector<Real,3> absNormal = Vector<Real,3>( fabs(Planes[i].Normal.X()), 
			fabs(Planes[i].Normal.Y()), fabs(Planes[i].Normal.Z()) );

		Real singedDist = Planes[i].DistanceTo(center);
		Real extent = Dot(absNormal, half);

		if (singedDist < -extent)
			return CT_Disjoint;
		else if (singedDist < extent)
			allInside = false;
	}
	return allInside ? CT_Contains : CT_Intersects;
}

