#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLRenderTargetView2D::OpenGLRenderTargetView2D(const shared_ptr<RHTexture>& texture, uint32_t arrIndex, uint32_t level)
	: OpenGLRenderView(texture),
	  mArrIndex(arrIndex),
	  mLevel(level)
{
}

void OpenGLRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnAttach(RHFrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnAttach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);

	OpenGLTexture* pTextureOGL = static_cast<OpenGLTexture*>(mTexture.get());

	if (GLEW_EXT_direct_state_access)
	{
		if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
		{
			//glNamedFramebufferTextureEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel);
			glNamedFramebufferTexture2DEXT(mFrameBufferOGL, attachment, GL_TEXTURE_2D, pTextureOGL->GetTextureOGL(), mLevel);
		}
		else 
		{
			glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
		}
	}
	else
	{
		GLuint oldFBO = OpenGLFrameBuffer::GetFBO();

		OpenGLFrameBuffer::BindFBO(mFrameBufferOGL);
		{
			if (pTextureOGL->GetTextureTarget() == GL_TEXTURE_2D)
			{
				//glFramebufferTexture(mFrameBufferID, attachment, texID, mLevel);
				glFramebufferTexture2D(mFrameBufferOGL, attachment, GL_TEXTURE_2D, pTextureOGL->GetTextureOGL(), mLevel);
			}
			else 
			{
				glFramebufferTextureLayer(mFrameBufferOGL, attachment, pTextureOGL->GetTextureOGL(), mLevel, mArrIndex);
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

void OpenGLRenderTargetView2D::OnDetach(RHFrameBuffer& fb, Attachment attr)
{
	OpenGLRenderView::OnDetach(fb, attr);

	GLenum attachment = GL_COLOR_ATTACHMENT0 + (attr - ATT_Color0);
	if (GLEW_EXT_direct_state_access)
	{
		if (mTexture->GetTextureArraySize() <= 1)
		{
			//glNamedFramebufferTextureEXT(mFrameBufferID, attachment, 0, 0);
			glNamedFramebufferTexture2DEXT(mFrameBufferOGL, attachment, GL_TEXTURE_2D, 0, 0);
		}
		else 
		{
			glNamedFramebufferTextureLayerEXT(mFrameBufferOGL, attachment, 0, 0, 0);
		}
	}
	else
	{
		GLuint oldFBO = OpenGLFrameBuffer::GetFBO();

		OpenGLFrameBuffer::BindFBO(mFrameBufferOGL);
		{
			if (mTexture->GetTextureArraySize() <= 1)
			{
				//glFramebufferTexture(mFrameBufferID, attachment, 0, 0);
				glFramebufferTexture2D(mFrameBufferOGL, attachment, 0, 0, 0);
			}
			else 
			{
				glFramebufferTextureLayer(mFrameBufferOGL, attachment, 0, 0, 0);
			}
		}
		OpenGLFrameBuffer::BindFBO(oldFBO);
	}

	OGL_ERROR_CHECK();
}

//////////////////////////////////////////////////////////////////////////
OpenGLScreenRenderTargetView2D::OpenGLScreenRenderTargetView2D()
	: OpenGLRenderView(nullptr)
{

}

void OpenGLScreenRenderTargetView2D::OnAttach(RHFrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnAttach(fb, attr);

	if(mFrameBufferOGL != 0)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenDepthStencilView Can Only Attach To Screen Frame Buffer",
			"OpenGLScreenRenderTargetView2D::OnAttach");
	}
}

void OpenGLScreenRenderTargetView2D::OnDetach(RHFrameBuffer& fb, Attachment attr)
{
	assert(attr == ATT_Color0);
	OpenGLRenderView::OnDetach(fb, attr);
}

void OpenGLScreenRenderTargetView2D::ClearColor( const ColorRGBA& clr )
{
	DoClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
}

}
