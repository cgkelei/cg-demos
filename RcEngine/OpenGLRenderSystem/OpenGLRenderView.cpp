#include "OpenGLRenderView.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "Core/Exception.h"
#include "Core/Context.h"

namespace RcEngine
{
	namespace RenderSystem
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

		void OpenGLRenderView::ClearStencil( unsigned int stencil )
		{

		}

		void OpenGLRenderView::ClearDepthStencil( float depth, unsigned int stencil )
		{

		}

		void OpenGLRenderView::OnAttached(FrameBuffer* fb, unsigned int att )
		{

		}

		void OpenGLRenderView::OnDetached(FrameBuffer* fb, unsigned int att )
		{

		}

		void OpenGLRenderView::DoClear( GLbitfield flag, const Math::ColorRGBA& clr, float depth, unsigned int stencil )
		{
			OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Core::Context::GetSingleton().GetRenderDevicePtr());

			FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();

			device->BindFrameBuffer(mAttachedFrameBuffer);


		}
	
	}
}







