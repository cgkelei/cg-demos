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

			void ToggleFullscreen(bool fs);
			bool IsFullscreen() const;

		protected:
			void DoBind();
			void DoUnbind();

		private:
			HDC  mHdc;
			HGLRC mHrc;

		};

	}
}

#endif // OpenGLRenderWindow_h__
