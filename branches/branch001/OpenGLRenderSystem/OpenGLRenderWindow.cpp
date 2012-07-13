#include "OpenGLRenderWindow.h"
#include "OpenGLRenderDevice.h"
#include "Core/Context.h"
#include "Core/Utility.h"
#include "MainApp/Application.h"
#include "MainApp/Window.h"

namespace RcEngine
{
	namespace Render
	{
		OpenGLRenderWindow::OpenGLRenderWindow( const RenderSettings& settings )
			: OpenGLFrameBuffer(settings.Width, settings.Height, false)
		{
			mColorFormat = settings.ColorFormat;
			mColorDepth = PixelFormatUtils::GetNumElemBits(settings.ColorFormat);
			mFullscreen = settings.Fullscreen;
			mIsDepthBuffered = PixelFormatUtils::IsDepthStencil(settings.DepthStencilFormat);
			PixelFormatUtils::GetNumDepthStencilBits(settings.DepthStencilFormat, mDepthBits, mStencilBits);

			Window* win = Core::Context::GetSingleton().GetApplication().GetMainWindow();

			win->UserResizedEvent.bind(this, &OpenGLRenderWindow::OnSize);


			mHwnd = Core::Context::GetSingleton().GetApplication().GetMainWindow()->GetHwnd();
			mHdc = GetDC(mHwnd);

			if (mFullscreen)												
			{
				mLeft = 0;
				mTop = 0;

				DEVMODE dmScreenSettings;								
				memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	
				dmScreenSettings.dmSize=sizeof(dmScreenSettings);	
				dmScreenSettings.dmPelsWidth	= mWidth;				
				dmScreenSettings.dmPelsHeight	= mHeight;				
				dmScreenSettings.dmBitsPerPel	= mColorDepth;					
				dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

				// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
				if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
				{
					assert(false);
				}

			}
			else
			{
				mColorDepth = ::GetDeviceCaps(mHdc, BITSPIXEL);
				mLeft = settings.Left;
				mTop = settings.Top;
			}

			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
			pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion	= 1;
			pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType	= PFD_TYPE_RGBA;
			pfd.cColorBits	= static_cast<BYTE>(mColorDepth);
			pfd.cDepthBits	= static_cast<BYTE>(mDepthBits);
			pfd.cStencilBits = static_cast<BYTE>(mStencilBits);
			pfd.iLayerType	= PFD_MAIN_PLANE;

			GLuint pixelFormat = ::ChoosePixelFormat(mHdc,&pfd);
			assert(pixelFormat != 0);

			::SetPixelFormat(mHdc,pixelFormat,&pfd);

			mHrc = ::wglCreateContext(mHdc);
			::wglMakeCurrent(mHdc, mHrc);

			unsigned int sampleCount = settings.SampleCount;
			if(sampleCount > 1)
			{

			}

			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			mViewport.mLeft = 0;
			mViewport.mTop = 0;
			mViewport.mWidth = mWidth;
			mViewport.mHeight = mHeight;

			std::ostringstream oss;
			oss << reinterpret_cast<char const *>(glGetString(GL_VENDOR)) << " "
				<< reinterpret_cast<char const *>(glGetString(GL_RENDERER)) << " " 
				<< reinterpret_cast<char const *>(glGetString(GL_VERSION));

			if (sampleCount > 1)
			{
				oss << " (" << sampleCount << "x AA)";
			}

			mDescription = oss.str();
			mActice = true;
		}

		OpenGLRenderWindow::~OpenGLRenderWindow()
		{

		}

		void OpenGLRenderWindow::ToggleFullscreen( bool fs )
		{

		}

		bool OpenGLRenderWindow::Fullscreen() const
		{
			return false;
		}

		void OpenGLRenderWindow::DoBind()
		{

		}

		void OpenGLRenderWindow::DoUnbind()
		{

		}

		void OpenGLRenderWindow::OnSize()
		{
			std::cout << "OnSize";
		}

		void OpenGLRenderWindow::WindowMovedOrResized()
		{
			RECT rect;
			::GetClientRect(mHwnd, &rect);

			unsigned int newLeft = rect.left;
			unsigned int newTop = rect.top;

			if ((newLeft != mLeft) || (newTop != mTop))
			{
				Core::Context::GetSingleton().GetApplication().GetMainWindow()->Reposition(newLeft, newTop);
			}

			unsigned int newWidth = rect.right - rect.left;
			unsigned int newHeight = rect.bottom - rect.top;

			if ((newWidth != mWidth) || (newHeight != mHeight))
			{
				Core::Context::GetSingleton().GetRenderDevice().Resize(newWidth, newHeight);
			}
		}

		void OpenGLRenderWindow::SwapBuffers()
		{
			::SwapBuffers(mHdc);
		}


	}
}