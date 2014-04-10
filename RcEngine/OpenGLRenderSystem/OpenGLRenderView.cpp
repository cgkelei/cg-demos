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
	mFrameBufferID = (static_cast_checked<OpenGLFrameBuffer*>(&fb))->GetFrameBufferObject();
	mAttachment = attr;
}

void OpenGLRenderView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	mAttachment = attr;
}

void OpenGLRenderView::DoClear( GLbitfield clearFlagOGL, const ColorRGBA& clr, float depth, uint32_t stencil )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	shared_ptr<FrameBuffer> currentFrameBuffer = device.GetCurrentFrameBuffer();
	OpenGLFrameBuffer& frameBufferOGL= *static_cast_checked<OpenGLFrameBuffer*>(currentFrameBuffer.get());
	assert(mFrameBufferID == frameBufferOGL.GetFrameBufferObject());

	const DepthStencilStateDesc& currDepthStencilDesc = device.GetCurrentDepthStencilState()->GetDesc();
	const BlendStateDesc& currBlendDesc = device.GetCurrentBlendState()->GetDesc();

	// mark all clear channel write mask true ,so we can clear it
	if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2 && mFrameBufferID != 0)
		{
			// separate render target blend enables and color write masks supported
			int32_t bufferIdx = mAttachment - ATT_Color0;
			if (currBlendDesc.RenderTarget[bufferIdx].ColorWriteMask != CWM_All)
				glColorMaskIndexedEXT(bufferIdx, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
		else
		{
			if (currBlendDesc.RenderTarget[0].ColorWriteMask != CWM_All)
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.DepthWriteMask)
			glDepthMask(GL_TRUE);
	}

	if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
	{
		if (!currDepthStencilDesc.StencilWriteMask)
			glStencilMask(GL_TRUE);
	}

	// do the clear
	if (GLEW_EXT_draw_buffers2 && mFrameBufferID != 0)
	{
		if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
		{
			GLint  bufferIndex = mAttachment - ATT_Color0;
			glClearBufferfv(GL_COLOR, bufferIndex, clr());
		}

		if ((clearFlagOGL & GL_DEPTH_BUFFER_BIT) && (clearFlagOGL & GL_STENCIL_BUFFER_BIT))
			glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
		else
		{
			if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
				glClearBufferfv(GL_DEPTH, 0, &depth);
			else
			{
				if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
				{
					GLint s = stencil;
					glClearBufferiv(GL_STENCIL, 0, &s);
				}
			}
		}
	}
	else
	{
		if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
			glClearColor(clr.R(), clr.G(), clr.B(), clr.A());

		if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
			glClearDepth(depth);

		if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
			glClearStencil(stencil);

		if (clearFlagOGL != 0)
			glClear(clearFlagOGL);
	}

	// set mask back
	if (clearFlagOGL & GL_COLOR_BUFFER_BIT)
	{
		if (GLEW_EXT_draw_buffers2 && mFrameBufferID != 0)
		{
			// separate render target blend enables and color write masks supported
			int32_t bufferIdx = mAttachment - ATT_Color0;
			uint32_t writeMask = currBlendDesc.RenderTarget[bufferIdx].ColorWriteMask;
			glColorMaskIndexedEXT(bufferIdx, (writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
				(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
		}
		else
		{
			uint32_t writeMask = currBlendDesc.RenderTarget[0].ColorWriteMask;
			glColorMask((writeMask & CWM_Red) != 0, (writeMask & CWM_Green) != 0, 
				(writeMask & CWM_Blue) != 0, (writeMask & CWM_Alpha) != 0);
		}
	}

	if (clearFlagOGL & GL_DEPTH_BUFFER_BIT)
	{
		if(!currDepthStencilDesc.DepthWriteMask)
			glDepthMask(GL_FALSE);
	}

	if (clearFlagOGL & GL_STENCIL_BUFFER_BIT)
		glStencilMask(currDepthStencilDesc.StencilWriteMask);

	OGL_ERROR_CHECK();
}
	







