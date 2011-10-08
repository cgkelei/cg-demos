//////////////////////////////////////////////////////////////////////////
template<typename Real>
Frustum3<Real>::Frustum3()
{

}


//////////////////////////////////////////////////////////////////////////
template<typename Real>
void Frustum3<Real>::Update( const Matrix4<Real>& viewProjection )
{
	// Left plane
	Planes[FP_Left].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M11, 
											  viewProjection.M24 + viewProjection.M21, 
											  viewProjection.M34 + viewProjection.M31);

	Planes[FP_Left].Constant = viewProjection.M44 + viewProjection.M41;
	

	// Right plane
	Planes[FP_Right].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M11, 
												viewProjection.M24 - viewProjection.M21, 
												viewProjection.M34 - viewProjection.M31);

	Planes[FP_Right].Constant= viewProjection.M44 - viewProjection.M41;

	// Top plane
	Planes[FP_Top].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M12, 
											viewProjection.M24 - viewProjection.M22, 
											viewProjection.M34 - viewProjection.M32);

	Planes[FP_Top].Constant = viewProjection.M44 - viewProjection.M42;


	// Bottom plane
	Planes[FP_Bottom].Normal = Vector<Real, 3>(viewProjection.M14 + viewProjection.M12, 
												viewProjection.M24 + viewProjection.M22, 
												viewProjection.M34 + viewProjection.M32);

	Planes[FP_Bottom].Constant = viewProjection.M44 + viewProjection.M42;

	// Near plane
	Planes[FP_Near].Normal = Vector<Real, 3>(viewProjection.M13, viewProjection.M23, viewProjection.M33);
	Planes[FP_Near].Constant = viewProjection.M43;


	// Far plane
	Planes[FP_Far].Normal = Vector<Real, 3>(viewProjection.M14 - viewProjection.M13,
											viewProjection.M24 - viewProjection.M23,
											viewProjection.M34 - viewProjection.M33);
	Planes[FP_Far].Constant = viewProjection.M44 - viewProjection.M43;

	// Normalize planes
	for ( int i = 0; i < 6; i++ )
	{
		Planes[i] = MathUtil::Normalize(Planes[i]);
	}
}


//////////////////////////////////////////////////////////////////////////
template<typename Real>
void Frustum3<Real>::Update(const Matrix4<Real>& view, const Matrix4<Real>& projection)
{
	Update(view * projection);
}