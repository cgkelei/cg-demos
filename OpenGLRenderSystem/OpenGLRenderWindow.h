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
	HDC  mHdc;
	HGLRC mHrc;
	HWND mHwnd;
	bool mFullscreen;
	std::string mDescription;
			

};

}

#endif // OpenGLRenderWindow_h__
