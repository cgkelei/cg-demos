#pragma once 
#ifndef _RenderSettings__H
#define _RenderSettings__H


#include "Prerequisites.h"
#include "PixelFormat.h"

namespace Engine
{
	struct _ApiExport RenderSettings 
	{
		RenderSettings()
			: Fullscreen(false),
			  Left(0),
			  Top(0),
			  ColorFormat(PF_A8R8G8B8),
			  SampleCount(1),
			  SampleQuality(0)
		{}
			  
			 
		bool Fullscreen;
		uint32 Left;
		uint32 Top;
		uint32 Width;
		uint32 Height;

		PixelFormat ColorFormat;
		DepthFormat DepthStencilForamt;

		uint32 SampleCount;
		uint32 SampleQuality;
		uint32 SyncInterval;

	};
}




#endif // _RenderSettings__H