#pragma once
#ifndef _Frustum__H
#define _Frustum__H

//  [8/24/2011 hustruan]

#include "MathUtil.h"

namespace Engine
{

	enum FrustumPlane
	{
		FP_Left = 0,
		FP_Right = 1,
		FP_Top = 2,
		FP_Bottom = 3,
		FP_Near = 4,
		FP_Far = 5,
		
	};

	template<typename Real>
	class Frustum3
	{
	public:
		Frustum3();

		/// <summary>
		/// Summary: Update the view frustum planes using the current view/projection matrices
		/// </summary>
		void Update(const Matrix4<Real>& view, const Matrix4<Real>& projection);
		void Update(const Matrix4<Real>& viewProjection);

	public:
		Plane3<Real> Planes[6];
	};

#include "Frustum.inl"

	typedef Frustum3<float> Frustum3f;


}

#endif