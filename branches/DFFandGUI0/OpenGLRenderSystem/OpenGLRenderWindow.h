#ifndef OpenGLRenderWindow_h__
#define OpenGLRenderWindow_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLFrameBuffer.h"

namespace RcEngine {

struct RenderSettings;

class _OpenGLExport OpenGLRenderWindow : public OpenGLFrameBuffer
{
public:
	OpenGLRenderWindow(const RenderSettings& settings);
	~OpenGLRenderWindow();

	void ToggleFullscreen(bool fs);
	bool Fullscreen() const;
	void SwapBuffers();
	void Resize(uint32_t width, uint32_t height);

protected:
	void DoBind();
	void DoUnbind();

private:
	int32_t mLeft, mTop;
	bool mFullscreen;
	std::string mDescription;

#ifdef RcWindows
	HDC  mHdc;
	HGLRC mHrc;
	HWND mHwnd;
#endif
			

};

}

#endif // OpenGLRenderWindow_h__
