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

	virtual void ToggleFullscreen(bool fs) = 0;
	virtual bool IsFullscreen() const = 0;

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
