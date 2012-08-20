#include "OpenGLRenderView.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include <Core/Context.h>
#include <Core/Exception.h>

namespace RcEngine {

OpenGLDepthStencilView::OpenGLDepthStencilView( uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, PixelFormat format )
	: mSampleCount(sampleCount), mSampleQuality(sampleQuality), mArrIndex(0), mLevel(-1), mDepthStencilTexture(NULL)
{
	//PixelFormatUtils::IsDepth()
	mWidth = width;
	mHeight = height;
	mFormat = format;

	GLint internalFormat;
	GLenum glformat;
	GLenum gltype;
	OpenGLMapping::Mapping(internalFormat, glformat, gltype, mFormat);

	glGenRenderbuffersEXT(1, &mRenderBufferObject);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderBufferObject);
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


OpenGLDepthStencilView::OpenGLDepthStencilView( Texture* tex, uint32_t arrIndex, uint32_t level )
{
	if(tex->GetTextureType() != TT_Texture2D)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only 2D Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	if(PixelFormatUtils::IsDepthStencil(tex->GetTextureFormat()))
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only Depth Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	mDepthStencilTexture = static_cast<OpenGLTexture2D*>(tex);
	mTarget = mDepthStencilTexture->GetOpenGLTextureTarget();

	mWidth = mDepthStencilTexture->GetWidth(level);
	mHeight = mDepthStencilTexture->GetHeight(level);
	mFormat = mDepthStencilTexture->GetTextureFormat();

}

OpenGLDepthStencilView::~OpenGLDepthStencilView()
{

}


void OpenGLDepthStencilView::OnAttach( FrameBuffer* fb, uint32_t index )
{
	assert(index == ATT_DepthStencil);
	mAttachedFrameBuffer = fb;

	OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Context::GetSingleton().GetRenderDevicePtr());
	FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();
	device->BindFrameBuffer(mAttachedFrameBuffer);

	if(mLevel < 0)
	{
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderBufferObject);
		if (PixelFormatUtils::IsStencil(mFormat))
		{
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRenderBufferObject);
		}
	}
	else
	{
		assert(mDepthStencilTexture != NULL);
		GLuint texID = mDepthStencilTexture->GetOpenGLTexture();

		if(mDepthStencilTexture->GetOpenGLTextureTarget() == GL_TEXTURE_2D)
		{
			if (PixelFormatUtils::IsDepthStencil(mFormat))
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
					mTarget, texID, mLevel);
			}
			if (PixelFormatUtils::IsStencil(mFormat))
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, 
					mTarget,texID, mLevel);
			}
		}
		else
		{
			// Texture Array
			if (PixelFormatUtils::IsDepthStencil(mFormat))
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texID, mArrIndex, mLevel);
			}
			if (PixelFormatUtils::IsStencil(mFormat))
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texID, mArrIndex, mLevel);
			}
		}
	}

	device->BindFrameBuffer(oldFrameBuffer);
}

void OpenGLDepthStencilView::OnDetach( FrameBuffer* fb, uint32_t index )
{
	assert(index == ATT_DepthStencil);
	assert(mAttachedFrameBuffer == fb);

	OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Context::GetSingleton().GetRenderDevicePtr());
	FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();
	device->BindFrameBuffer(mAttachedFrameBuffer);

	if(mLevel < 0)
	{
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
	}
	else
	{
		assert(mDepthStencilTexture != NULL);
		GLuint texID = mDepthStencilTexture->GetOpenGLTexture();

		if(mDepthStencilTexture->GetOpenGLTextureTarget() == GL_TEXTURE_2D)
		{
			if (PixelFormatUtils::IsDepthStencil(mFormat))
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
					mTarget, 0, 0);
			}
			if (PixelFormatUtils::IsStencil(mFormat))
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, 
					mTarget,0, 0);
			}
		}
		else
		{
			// Texture Array
			if (PixelFormatUtils::IsDepthStencil(mFormat))
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0, 0);
			}
			if (PixelFormatUtils::IsStencil(mFormat))
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0, 0);
			}
		}
	}

	device->BindFrameBuffer(oldFrameBuffer);
	mAttachedFrameBuffer = NULL;

}

void OpenGLDepthStencilView::ClearDepth( float depth )
{

}

void OpenGLDepthStencilView::ClearStencil( uint32_t stencil )
{

}

void OpenGLDepthStencilView::ClearDepthStencil( float depth, uint32_t stencil )
{

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

void OpenGLScreenDepthStencilView::OnAttach( FrameBuffer* fb, uint32_t index )
{
	OpenGLFrameBuffer* oglFrameBuffer = static_cast<OpenGLFrameBuffer*>(fb);
	if(oglFrameBuffer->GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnAttached");
	}
	mAttachedFrameBuffer = fb;

}

void OpenGLScreenDepthStencilView::OnDetach( FrameBuffer* fb, uint32_t index )
{
	OpenGLFrameBuffer* oglFrameBuffer = static_cast<OpenGLFrameBuffer*>(fb);
	if(oglFrameBuffer->GetFrameBufferObject() != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnDetached");
	}
	mAttachedFrameBuffer = NULL;
}

}