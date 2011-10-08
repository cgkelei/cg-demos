// [7/31/2011 hustruan]

#pragma once
#ifndef _Viewport__H
#define _Viewport__H

#include "Prerequisites.h"




namespace Engine
{

	class _ApiExport Viewport
	{
	public:
		Viewport(RenderTarget* target);
		Viewport(RenderTarget* target, int32 left, int32 top, int32 width, int32 height);
		~Viewport();

		RenderTarget* GetTarget() const;
		Camera& GetCamera();

		/// <summary>
		/// Update viewport, can not call directly
		/// </summary>
		void UpdateDimensions(void);
		
		/// <summary>
		/// Instructs the viewport to updates its contents.
		/// </summary>
		void Update();

	protected:

		RenderTarget* mTarget;

		Camera* mCamera;

		uint32 mLeft, mTop, mWidth, mHeight;

	};
}



#endif // _Viewport__H