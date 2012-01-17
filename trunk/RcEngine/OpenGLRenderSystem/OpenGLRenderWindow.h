#ifndef OpenGLRenderWindow_h__
#define OpenGLRenderWindow_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/RenderWindow.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		class _OpenGLExport OpenGLRenderWindow : public RenderWindow
		{
		public:
			OpenGLRenderWindow(const RenderSettings& settings);
			~OpenGLRenderWindow();

		private:
			HDC  mHdc;
			HGLRC mHrc;

		};

	}
}

#endif // OpenGLRenderWindow_h__
