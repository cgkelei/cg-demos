#include "OpenGLRenderView.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLTexture.h"
#include <Core/Exception.h>
#include <Core/Context.h>

namespace RcEngine {

OpenGLRenderTargetView2D::OpenGLRenderTargetView2D(Texture& texture, uint32_t arrIndex, uint32_t level)
	: mTextureOGL(*(static_cast_checked<OpenGLTexture2D*>(&texture))),
	  mArrIndex(arrIndex), mLevel(level)
{
	mWidth = mTextureOGL.GetWidth(level);
	mHeight = mTextureOGL.GetHeight(level);
	mFormat = mTextureOGL.GetTextureFormat();
}

OpenGLRenderTargetView2D::~OpenGLRenderTargetView2D()
{

}

void OpenGLRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
}

void OpenGLRenderTargetView2D::OnAttach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnAttach(fb, attr);

	uint32_t index = attr - ATT_Color0;

	if(mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
	{		
		if(mTextureOGL.GetSampleCount() <= 1)
		{
			
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				GL_COLOR_ATTACHMENT0_EXT + index, GL_TEXTURE_2D, mTextureOGL.GetOpenGLTexture(), mLevel);
		}
		else
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
				GL_COLOR_ATTACHMENT0_EXT + index, GL_RENDERBUFFER_EXT, mTextureOGL.GetOpenGLTexture());
		}
	}
	else
	{
		// 2D Texture Array
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
			mTextureOGL.GetOpenGLTexture(), mArrIndex, mLevel);
	}
}

void OpenGLRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnDetach(fb, attr);

	uint32_t index = attr - ATT_Color0;

	if(mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
	{		
		if(mTextureOGL.GetSampleCount() <= 1)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index, GL_TEXTURE_2D, 0, 0);
		}
		else
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + index, GL_RENDERBUFFER_EXT, 0);
		}
	}
	else
	{
		// 2D Texture Array
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, 0, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
OpenGLScreenRenderTargetView2D::OpenGLScreenRenderTargetView2D( uint32_t width, uint32_t height, PixelFormat fmt )
{
	mWidth = width;
	mHeight = height;
	mFormat = fmt;
}

OpenGLScreenRenderTargetView2D::~OpenGLScreenRenderTargetView2D()
{

}

void OpenGLScreenRenderTargetView2D::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnAttach(fb, attr);

	OpenGLFrameBuffer& framBufferOGL = *(static_cast_checked<OpenGLFrameBuffer*>(&fb));

	if(framBufferOGL.GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenRenderTargetView2D::OnAttach");
	}
}

void OpenGLScreenRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnDetach(fb, attr);

	OpenGLFrameBuffer& framBufferOGL = *(static_cast_checked<OpenGLFrameBuffer*>(&fb));
	if(framBufferOGL.GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "OpenGLScreenRenderTarget2DView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenRenderTargetView2D::OnDetach");
	}
}

void OpenGLScreenRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
}

}
