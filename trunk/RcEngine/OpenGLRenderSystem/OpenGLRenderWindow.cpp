#include "OpenGLRenderWindow.h"
#include "OpenGLDevice.h"
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Exception.h>
#include <Core/Utility.h>

namespace RcEngine {

OpenGLRenderWindow::OpenGLRenderWindow( const RenderSettings& settings )
	: OpenGLFrameBuffer(settings.Width, settings.Height, false)
{
	mFullscreen = settings.Fullscreen;

	uint32_t colorDepth, depthBits, stencilBits;

	colorDepth = PixelFormatUtils::GetNumElemBits(settings.ColorFormat);
	PixelFormatUtils::GetNumDepthStencilBits(settings.DepthStencilFormat, depthBits, stencilBits);

	bool isDepthBuffered = PixelFormatUtils::IsDepth(settings.DepthStencilFormat);

#ifdef RcWindows
	Window* mainWindow = Context::GetSingleton().GetApplication().GetMainWindow();
	mHwnd = mainWindow->GetHwnd();
	mHdc = GetDC(mHwnd);
	
	if (mFullscreen)												
	{
		mLeft = 0;
		mTop = 0;

		DEVMODE dmScreenSettings;								
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);	
		dmScreenSettings.dmPelsWidth	= settings.Width;				
		dmScreenSettings.dmPelsHeight	= settings.Height;				
		dmScreenSettings.dmBitsPerPel	= colorDepth;					
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			assert(false);
		}

	}
	else
	{
		colorDepth = ::GetDeviceCaps(mHdc, BITSPIXEL);
		mLeft = settings.Left;
		mTop = settings.Top;
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= static_cast<BYTE>(colorDepth);
	pfd.cDepthBits	= static_cast<BYTE>(depthBits);
	pfd.cStencilBits = static_cast<BYTE>(stencilBits);
	pfd.iLayerType	= PFD_MAIN_PLANE;

	GLuint pixelFormat = ::ChoosePixelFormat(mHdc,&pfd);
	assert(pixelFormat != 0);

	::SetPixelFormat(mHdc,pixelFormat,&pfd);

	mHrc = ::wglCreateContext(mHdc);
	::wglMakeCurrent(mHdc, mHrc);
	
	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		String errMsg = reinterpret_cast<char const *>(glewGetErrorString(err));
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, errMsg, "OpenGLRenderDevice::InitGlew");
	}

	if (WGLEW_EXT_swap_control)
		wglSwapIntervalEXT(settings.SyncInterval);
#endif

	uint32_t sampleCount = settings.SampleCount;
	if(sampleCount > 1)
	{

	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::ostringstream oss;
	oss << reinterpret_cast<char const *>(glGetString(GL_VENDOR)) << " "
		<< reinterpret_cast<char const *>(glGetString(GL_RENDERER)) << " " 
		<< reinterpret_cast<char const *>(glGetString(GL_VERSION));

	if (sampleCount > 1)
	{
		oss << " (" << sampleCount << "x AA)";
	}

	mDescription = oss.str();
}

OpenGLRenderWindow::~OpenGLRenderWindow()
{
	Window* win = Context::GetSingleton().GetApplication().GetMainWindow();
}

void OpenGLRenderWindow::ToggleFullscreen( bool fs )
{

}

bool OpenGLRenderWindow::Fullscreen() const
{
	return false;
}

void OpenGLRenderWindow::OnBind()
{
	gOpenGLDevice->BindFBO(0);

	GLenum targets[] = { GL_BACK };
	glDrawBuffers(1, &targets[0]);

	OGL_ERROR_CHECK();
}

void OpenGLRenderWindow::OnUnbind()
{

}

void OpenGLRenderWindow::SwapBuffers()
{
#ifdef RcWindows
	::SwapBuffers(mHdc);
#endif
}

}
