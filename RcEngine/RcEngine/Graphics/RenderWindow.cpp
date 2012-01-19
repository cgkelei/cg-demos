#include "Graphics/RenderWindow.h"
#include "Graphics/RenderSettings.h"
#include "Graphics/RenderDevice.h"
#include "MainApp/Window.h"
#include "MainApp/Application.h"
#include "Core/Context.h"

namespace RcEngine {
namespace RenderSystem {

RenderWindow::RenderWindow( const RenderSettings& settings )
	: FrameBuffer(true)
{
	mWidth = settings.Width;
	mHeight = settings.Height;
	mColorFormat = settings.ColorFormat;
	mColorDepth = PixelFormatUtils::GetNumElemBits(settings.ColorFormat);
	mFullscreen = settings.Fullscreen;
	mIsDepthBuffered = PixelFormatUtils::IsDepthStencil(settings.DepthStencilFormat);
	PixelFormatUtils::GetNumDepthStencilBits(settings.DepthStencilFormat, mDepthBits, mStencilBits);
	
	Window* win = Core::Context::GetSingleton().GetApplication().GetMainWindow();

	win->UserResizedEvent.bind(this, &RenderWindow::OnSize);
}


RenderWindow::~RenderWindow(void)
{
}

void RenderWindow::OnSize()
{

}

void RenderWindow::WindowMovedOrResized()
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


} // RenderSystem
} // RcEngine

