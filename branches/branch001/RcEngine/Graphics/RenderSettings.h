#ifndef RenderSettings_h__
#define RenderSettings_h__

#include "PixelFormat.h"

namespace RcEngine {
namespace Render {

struct RenderSettings
{
	bool Fullscreen;
	unsigned int Left, Top;
	unsigned int Width, Height;
	PixelFormat ColorFormat;
	PixelFormat DepthStencilFormat;
	unsigned int SampleCount, SampleQuality;
};

} // RenderSystem
} // RcEngine


#endif // RenderSettings_h__
