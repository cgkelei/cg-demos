#include "OpenGLRenderView.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLTexture.h"
#include <Core/Exception.h>
#include <Core/Context.h>

namespace RcEngine {

OpenGLRenderTargetView2D::OpenGLRenderTargetView2D(Texture& texture, uint32_t arrIndex, uint32_t level)
	: mTextureOGL(*(static_cast_checked<OpenGLTexture2D*>(&texture))),
	  mArrIndex(arrIndex),
	  mLevel(level)
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
	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnAttach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnAttach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);

	if (GLEW_EXT_direct_state_access)
	{
		if (mTextureOGL.RenderBufferHint())
		{
			GLuint renderBufferID = mTextureOGL.GetOpenGLTexture();
			glNamedFramebufferRenderbufferEXT(mFrameBufferID, attachment, GL_RENDERBUFFER, renderBufferID);
		}
		else
		{
			GLuint texID = mTextureOGL.GetOpenGLTexture();
			if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
			{
				//glNamedFramebufferTextureEXT(mFrameBufferID, attachment, texID, mLevel);
				glNamedFramebufferTexture2DEXT(mFrameBufferID, attachment, mTextureOGL.GetOpenGLTextureTarget(), texID, mLevel);
			}
			else 
			{
				glNamedFramebufferTextureLayerEXT(mFrameBufferID, attachment, texID, mLevel, mArrIndex);
			}
		}
	}
	else
	{
		GLuint oldFBO = OpenGLFrameBuffer::GetFBO();

		OpenGLFrameBuffer::BindFBO(mFrameBufferID);
		{
			if (mTextureOGL.RenderBufferHint())
			{
				GLuint renderBufferID = mTextureOGL.GetOpenGLTexture();
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderBufferID);
			}
			else
			{
				GLuint texID = mTextureOGL.GetOpenGLTexture();
				if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
				{
					//glFramebufferTexture(mFrameBufferID, attachment, texID, mLevel);
					glFramebufferTexture2D(mFrameBufferID, attachment, mTextureOGL.GetOpenGLTextureTarget(), texID, mLevel);
				}
				else 
				{
					glFramebufferTextureLayer(mFrameBufferID, attachment, texID, mLevel, mArrIndex);
				}
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnDetach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);
	if (GLEW_EXT_direct_state_access)
	{
		if (mTextureOGL.RenderBufferHint())
			glNamedFramebufferRenderbufferEXT(mFrameBufferID, attachment, GL_RENDERBUFFER, 0);
		else
		{
			if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
			{
				//glNamedFramebufferTextureEXT(mFrameBufferID, attachment, 0, 0);
				glNamedFramebufferTexture2DEXT(mFrameBufferID, attachment, mTextureOGL.GetOpenGLTextureTarget(), 0, 0);
			}
			else 
			{
				glNamedFramebufferTextureLayerEXT(mFrameBufferID, attachment, 0, 0, 0);
			}
		}
	}
	else
	{
		GLuint oldFBO = OpenGLFrameBuffer::GetFBO();

		OpenGLFrameBuffer::BindFBO(mFrameBufferID);
		{
			if (mTextureOGL.RenderBufferHint())
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, 0);
			}
			else
			{
				if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
				{
					//glFramebufferTexture(mFrameBufferID, attachment, 0, 0);
					glFramebufferTexture2D(mFrameBufferID, attachment, 0, 0, 0);
				}
				else 
				{
					glFramebufferTextureLayer(mFrameBufferID, attachment, 0, 0, 0);
				}
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
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

	if(mFrameBufferID != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenRenderTargetView2D::OnAttach");
	}
}

void OpenGLScreenRenderTargetView2D::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnDetach(fb, attr);
}

void OpenGLScreenRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
}

}
