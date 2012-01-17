#ifndef RenderWindow_h__
#define RenderWindow_h__

#include "Core/Prerequisites.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderSettings.h"

namespace RcEngine {
namespace RenderSystem {

class _ApiExport RenderWindow : public FrameBuffer
{
public:
	RenderWindow(const RenderSettings& setting);
	virtual ~RenderWindow(void);

	void OnSize();

protected:
	void WindowMovedOrResized();

protected:
	bool mFullscreen;
	std::string mDescription;
	HWND mHwnd;

};

} // RenderSystem
} // RcEngine

#endif // RenderWindow_h__
