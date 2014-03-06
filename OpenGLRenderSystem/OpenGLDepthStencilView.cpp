#include "OpenGLRenderView.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include <Core/Context.h>
#include <Core/Exception.h>

namespace RcEngine {

OpenGLDepthStencilView::OpenGLDepthStencilView( Texture& texture, uint32_t arrIndex, uint32_t level )
	: mTextureOGL(*(static_cast_checked<OpenGLTexture2D*>(&texture))),
	  mArrIndex(arrIndex),
	  mLevel(level)
{
	if(mTextureOGL.GetTextureType() != TT_Texture2D)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only 2D Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	if(!PixelFormatUtils::IsDepth(texture.GetTextureFormat()))
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only Depth Texture Needed",
			"OpenGLDepthStencilView::OpenGLDepthStencilView");
	}

	mWidth = texture.GetWidth(level);
	mHeight = texture.GetHeight(level);
	mFormat = texture.GetTextureFormat();
}

OpenGLDepthStencilView::~OpenGLDepthStencilView()
{
	
}

void OpenGLDepthStencilView::OnAttach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnAttach(fb, attr);

	GLenum attachment = GL_DEPTH_ATTACHMENT;
	if (PixelFormatUtils::IsStencil(mFormat))
		attachment = GL_DEPTH_STENCIL_ATTACHMENT;

	if (GLEW_EXT_direct_state_access)
	{
		if(mTextureOGL.RenderBufferHint())  
		{
			GLuint renderBufferID = mTextureOGL.GetOpenGLTexture();
			glNamedFramebufferRenderbufferEXT(mFrameBufferID, attachment, GL_RENDERBUFFER, renderBufferID);
		}
		else
		{
			GLuint texID = mTextureOGL.GetOpenGLTexture();
			if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
				glNamedFramebufferTextureEXT(mFrameBufferID, attachment, texID, mLevel);
			else 
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::Attach");
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
					glFramebufferTexture(mFrameBufferID, attachment, texID, mLevel);
				else 
				{
					ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::Attach");
					glFramebufferTextureLayer(mFrameBufferID, attachment, texID, mLevel, mArrIndex);
				}
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLDepthStencilView::OnDetach(FrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_DepthStencil);
	OpenGLRenderView::OnDetach(fb, attr);

	if (GLEW_EXT_direct_state_access)
	{
		if(mTextureOGL.RenderBufferHint())  
		{
			glNamedFramebufferRenderbufferEXT(mFrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
		}
		else
		{
			if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
				glNamedFramebufferTextureEXT(mFrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
			else 
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::OnDetach");
				glNamedFramebufferTextureLayerEXT(mFrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0, 0);
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
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
			}
			else
			{
				if (mTextureOGL.GetOpenGLTextureTarget() == GL_TEXTURE_2D)
					glFramebufferTexture(mFrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
				else 
				{
					ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Depth Texture Array May Error!", "OpenGLDepthStencilView::OnDetach");
					glFramebufferTextureLayer(mFrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0, 0);
				}
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLDepthStencilView::ClearDepth(float depth)
{
	DoClear(GL_DEPTH_BUFFER_BIT, ColorRGBA(), depth, 0);
}

void OpenGLDepthStencilView::ClearStencil(uint32_t stencil)
{
	DoClear(GL_STENCIL_BUFFER_BIT, ColorRGBA(), 0, stencil);
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

	if(mFrameBufferID != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenDepthStencilView::OnAttach");
	}
}

void OpenGLScreenDepthStencilView::OnDetach( FrameBuffer& fb, Attachment attr )
{
	OpenGLRenderView::OnDetach(fb, attr);
	
	if(mFrameBufferID != 0)
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