#ifndef RenderSettings_h__
#define RenderSettings_h__

#include <Graphics/PixelFormat.h>

namespace RcEngine {
namespace Render {

struct RenderSettings
{
	bool Fullscreen;
	int32_t Left, Top;
	uint32_t Width, Height;
	PixelFormat ColorFormat;
	PixelFormat DepthStencilFormat;
	uint32_t SampleCount, SampleQuality;
};

} // Namespace Render
} // Namespace RcEngine


#endif // RenderSettings_h__
