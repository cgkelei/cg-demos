#pragma once
#ifndef _Camera__H
#define _Camera__H

// [7/31/2011 hustruan]

#include "Prerequisites.h"

namespace Engine
{

	/** Worldspace clipping planes.
	*/
	enum FrustumPlane
	{
		Frustum_Plane_Near   = 0,
		Frustum_Plane_Far    = 1,
		Frustum_Plane_Left   = 2,
		Frustum_Plane_Right  = 3,
		Frustum_Plane_Top    = 4,
		Frustum_Plane_Bottom = 5
	};


	class _ApiExport Camera
	{
	public:
		Camera(void);
		virtual ~Camera(void);

		void RenderScreen(Viewport* vp);
	};
}




#endif // _Camera__H