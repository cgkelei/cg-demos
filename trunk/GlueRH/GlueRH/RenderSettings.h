#ifndef _RenderSettings__H
#define _RenderSettings__H

#include <DXGI.h>

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
		int		Left;
		int		Top;
		int		Width;
		int		Height;
		PiexelFormat ColorFormat;
		DepthFormat DepthStencilFormat;
		int SampleCount;
		int SampleQuality ;
		
	};
}


#endif