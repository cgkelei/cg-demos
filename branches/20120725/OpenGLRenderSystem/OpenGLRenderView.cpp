#include "OpenGLRenderView.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "Core/Exception.h"
#include "Core/Context.h"

namespace RcEngine
{
	namespace Render
	{

		OpenGLRenderView::OpenGLRenderView(void)
		{
		}


		OpenGLRenderView::~OpenGLRenderView(void)
		{
		}

		void OpenGLRenderView::ClearColor( const Math::ColorRGBA& clearColor )
		{
	
		}

		void OpenGLRenderView::ClearDepth( float depth )
		{

		}

		void OpenGLRenderView::ClearStencil( uint32_t stencil )
		{

		}

		void OpenGLRenderView::ClearDepthStencil( float depth, uint32_t stencil )
		{

		}

		void OpenGLRenderView::OnAttached(FrameBuffer* fb, uint32_t att )
		{

		}

		void OpenGLRenderView::OnDetached(FrameBuffer* fb, uint32_t att )
		{

		}

		void OpenGLRenderView::DoClear( GLbitfield flag, const Math::ColorRGBA& clr, float depth, uint32_t stencil )
		{
			OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Core::Context::GetSingleton().GetRenderDevicePtr());
			FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();
			device->BindFrameBuffer(mAttachedFrameBuffer);
		}
	
	}
}






