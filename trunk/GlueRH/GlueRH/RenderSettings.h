#ifndef _RenderSettings__H
#define _RenderSettings__H

#include "Common.h"

namespace GlueRH
{
	struct RenderSettings
	{
		RenderSettings()
			: FullScreen(false),
			Left(0), Top(0),
			ColorFormat(DXGI_FORMAT_R8G8B8A8_UNORM), 
			DepthStencilFormat(DXGI_FORMAT_D16_UNORM),
			SampleCount(1),
			SampleQuality(0)	
		{

		}

		bool	FullScreen;
		int32		Left;
		int32		Top;
		int32		Width;
		int32		Height;
		DXGI_FORMAT ColorFormat;
		DXGI_FORMAT DepthStencilFormat;
		int32 SampleCount;
		int32 SampleQuality ;
		
	};
}


#endif