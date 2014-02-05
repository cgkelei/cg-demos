#include "OpenGLRenderView.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include <Core/Context.h>
#include <Core/Exception.h>

namespace RcEngine {

OpenGLDepthStencilView::OpenGLDepthStencilView( uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, PixelFormat format )
	: mSampleCount(sampleCount), mSampleQuality(sampleQuality), mArrIndex(0), mLevel(-1)
{
	//PixelFormatUtils::IsDepth()
	mWidth = width;
	mHeight = height;
	mFormat = format;

	GLint internalFormat;
	GLenum glformat;
	GLenum gltype;
	OpenGLMapping::Mapping(internalFormat, glformat, gltype, mFormat);

	glGenRenderbuffersEXT(1, &mRenderBufferID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderBufferID);
	if (sampleCount <= 1)
	{
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
			internalFormat, mWidth, mHeight);
	}
	else
	{
		glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, sampleCount,
			internalFormat, mWidth, mHeight);
	}
}


OpenGLDepthStencilView::OpenGLDepthStencilView( Texture& texture, uint32_t arrIndex, uint32_t level )
	: mArrIndex(arrIndex), mLevel(level)
{
	if(texture.GetTextureType() != TT_Texture2D)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only 2D Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	if(!PixelFormatUtils::IsDepth(texture.GetTextureFormat()))
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only Depth Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	OpenGLTexture& textureOGL = *(static_cast_checked<OpenGLTexture*>(&texture));

	mFormat = textureOGL.GetTextureFormat();
	mWidth = textureOGL.GetWidth(mLevel);
	mHeight = textureOGL.GetHeight(mLevel);

	mTextureTarget = textureOGL.GetOpenGLTextureTarget();
	mTextureID = textureOGL.GetOpenGLTexture();

	mSampleCount = textureOGL.GetSampleCount();
	mSampleQuality = textureOGL.GetSampleQuality();
}

OpenGLDepthStencilView::~OpenGLDepthStencilView()
{
	
}


void OpenGLDepthStencilView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnAttach(fb, attr);

	if(mLevel < 0)
	{
		// use render buffer
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderBufferID);
		if (PixelFormatUtils::IsStencil(mFormat))
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderBufferID);
		}
	}
	else
	{
		// has mip map, use textue object
		if(mTextureTarget == GL_TEXTURE_2D)
		{
			if (mFormat == PF_Depth24Stencil8)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mTextureTarget, mTextureID, mLevel);
			else if (mFormat == PF_Depth16 || mFormat == PF_Depth32)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mTextureTarget, mTextureID, mLevel);
		}
		else
		{
			if (mFormat == PF_Depth24Stencil8)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mTextureID, mArrIndex, mLevel);
			else if (mFormat == PF_Depth16 || mFormat == PF_Depth32)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mTextureID, mArrIndex, mLevel);
		}
	}
}

void OpenGLDepthStencilView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnDetach(fb, attr);

	if(mLevel < 0)
	{
		// use render buffer
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
		if (PixelFormatUtils::IsStencil(mFormat))
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
		}
	}
	else
	{
		if(mTextureTarget == GL_TEXTURE_2D)
		{
			if (mFormat == PF_Depth24Stencil8)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, mTextureTarget, 0, 0);
			else if (mFormat == PF_Depth16 || mFormat == PF_Depth32)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mTextureTarget, 0, 0);
		}
		else
		{
			if (mFormat == PF_Depth24Stencil8)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0, 0);
			else if (mFormat == PF_Depth16 || mFormat == PF_Depth32)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0, 0);
		}
	}
}

void OpenGLDepthStencilView::ClearDepth(float depth)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), depth, 0);
}

void OpenGLDepthStencilView::ClearStencil(uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), 0, stencil);
}

void OpenGLDepthStencilView::ClearDepthStencil(float depth, uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, ColorRGBA(), depth, stencil);
}

//////////////////////////////////////////////////////////////////////////
OpenGLScreenDepthStencilView::OpenGLScreenDepthStencilView( uint32_t width, uint32_t height, PixelFormat fmt )
{
	mWidth = width;
	mHeight = height;
	mFormat = fmt;
}

OpenGLScreenDepthStencilView::~OpenGLScreenDepthStencilView()
{

}

void OpenGLScreenDepthStencilView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnAttach(fb, attr);

	OpenGLFrameBuffer& framBufferOGL = *(static_cast_checked<OpenGLFrameBuffer*>(&fb));
	if(framBufferOGL.GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnAttach");
	}
}

void OpenGLScreenDepthStencilView::OnDetach( FrameBuffer& fb, Attachment attr )
{
	OpenGLFrameBuffer& framBufferOGL = *(static_cast_checked<OpenGLFrameBuffer*>(&fb));

	if(framBufferOGL.GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnDetached");
	}
}

void OpenGLScreenDepthStencilView::ClearDepth(float depth)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), depth, 0);
}

void OpenGLScreenDepthStencilView::ClearStencil(uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), 0, stencil);
}

void OpenGLScreenDepthStencilView::ClearDepthStencil(float depth, uint32_t stencil)
{
	DoClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, ColorRGBA(), depth, stencil);
}

}