#include "OpenGLRenderView.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include <Core/Exception.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/BlendState.h>
#include <Core/Context.h>

namespace RcEngine {

OpenGLRenderView::OpenGLRenderView(void)
{
}


OpenGLRenderView::~OpenGLRenderView(void)
{
}

void OpenGLRenderView::ClearColor( const ColorRGBA& clearColor )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearColor");
}

void OpenGLRenderView::ClearDepth( float depth )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearDepth");
}

void OpenGLRenderView::ClearStencil( uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearStencil");
}

void OpenGLRenderView::ClearDepthStencil( float depth, uint32_t stencil )
{
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Shouldn't Go There", "OpenGLRenderView::ClearDepthStencil");
}

void OpenGLRenderView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	shared_ptr<FrameBuffer> currentFrameBuffer = Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer();
	assert( &fb == currentFrameBuffer.get());

	mFrameBufferID = (static_cast_checked<OpenGLFrameBuffer*>(&fb))->GetFrameBufferObject();
	mAttachment = attr;

	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBufferID);
}

void OpenGLRenderView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	/*shared_ptr<FrameBuffer> currentFrameBuffer = Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer();
	assert( &fb == currentFrameBuffer.get());*/

	mFrameBufferID = (static_cast_checked<OpenGLFrameBuffer*>(&fb))->GetFrameBufferObject();
	mAttachment = attr;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBufferID);
}

void OpenGLRenderView::DoClear( GLbitfield flags, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	OpenGLFrameBuffer& frameBufferOGL= *static_cast_checked<OpenGLFrameBuffer*>(currentFrameBuffer.get());
	assert(mFrameBufferID == frameBufferOGL.GetFrameBufferObject());

	const DepthStencilStateDesc& currDepthStencilDesc = device.GetCurrentDepthStencilState()->GetDesc();
	const BlendStateDesc& currBlendDesc = device.GetCurrentBlendState()->GetDesc();

	GLbitfield clearFlagOGL = 0;

	// mark all clear channel write mask true ,so we can clear it
	if (flags & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2)
		{
			// separate render target blend enables and color write masks supported
			for (size_t i = 0; i < 8; ++i)
			{
				if (currBlendDesc.RenderTarget[i].ColorWriteMask != CWM_All)
				{
					glColorMaskIndexedEXT(i, true, true, true, true);
				}
			}
		}
		else
		{
			if (currBlendDesc.RenderTarget[0].ColorWriteMask != CWM_All)
			{
				glColorMask(true, true, true, true);
			}
		}
	}

	if (flags & GL_DEPTH_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.DepthWriteMask)
		{
			glDepthMask(GL_TRUE);
		}
	}

	if (flags & GL_STENCIL_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.StencilWriteMask)
		{
			glStencilMask(GL_TRUE);
		}
	}

	// do the clear
	uint32_t bufferIndex = mAttachment - ATT_DepthStencil;
	if (GLEW_EXT_draw_buffers2 && mFrameBufferID != 0)
	{
		if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
			glClearBufferfv(GL_COLOR, bufferIndex, clr());

		if ((clearFlagOGL & GL_DEPTH_BUFFER_BIT) && (clearFlagOGL & GL_STENCIL_BUFFER_BIT))
			glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
		else
		{
			if (flags & GL_DEPTH_BUFFER_BIT)
				glClearBufferfv(GL_DEPTH, 0, &depth);
			else
			{
				if (flags & GL_STENCIL_BUFFER_BIT)
				{
					GLint s = stencil;
					glClearBufferiv(GL_STENCIL, 0, &s);
				}
			}
		}
	}
	else
	{
		if (flags & GL_COLOR_BUFFER_BIT)
			glClearColor(clr.R(), clr.G(), clr.B(), clr.A());

		if (flags & GL_DEPTH_BUFFER_BIT)
			glClearDepth(depth);

		if (flags & GL_STENCIL_BUFFER_BIT)
			glClearStencil(stencil);

		if (flags != 0)
			glClear(flags);
	}

	// set mask back
	if (flags & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2)
		{
			// separate render target blend enables and color write masks supported
			for (size_t i = 0; i < 8; ++i)
			{
				uint32_t writeMask = currBlendDesc.RenderTarget[i].ColorWriteMask;
				glColorMaskIndexedEXT(i, (writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
					(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
			}
		}
		else
		{
			uint32_t writeMask = currBlendDesc.RenderTarget[0].ColorWriteMask;
			glColorMask((writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
				(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
		}
	}

	if (flags & GL_DEPTH_BUFFER_BIT)
	{
		if(!currDepthStencilDesc.DepthWriteMask)
			glDepthMask(GL_FALSE);
	}

	if (flags & GL_STENCIL_BUFFER_BIT)
	{
		if (currDepthStencilDesc.StencilWriteMask)
			glStencilMask(GL_FALSE);
	}
}
	


}







