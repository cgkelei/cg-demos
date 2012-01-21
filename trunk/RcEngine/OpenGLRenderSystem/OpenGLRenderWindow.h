#ifndef OpenGLRenderWindow_h__
#define OpenGLRenderWindow_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLFrameBuffer.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		class _OpenGLExport OpenGLRenderWindow : public OpenGLFrameBuffer
		{
		public:
			OpenGLRenderWindow(const RenderSettings& settings);
			~OpenGLRenderWindow();

			void ToggleFullscreen(bool fs);
			bool Fullscreen() const;
			void SwapBuffers();

		protected:
			void DoBind();
			void DoUnbind();

			void OnSize();
			void WindowMovedOrResized();

		private:
			HDC  mHdc;
			HGLRC mHrc;
			HWND mHwnd;
			bool mFullscreen;
			std::string mDescription;
			

		};

	}
}

#endif // OpenGLRenderWindow_h__
