#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLRenderView.h"
#include <Core/Exception.h>
#include <Core/Context.h>

namespace RcEngine {

GLuint OpenGLFrameBuffer::msCurrFrameBufferObject = 0;

void OpenGLFrameBuffer::BindFBO( GLuint fbo )
{
	if (fbo != msCurrFrameBufferObject)
	{
		msCurrFrameBufferObject = fbo;
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
}

GLuint OpenGLFrameBuffer::GetFBO()
{
	return msCurrFrameBufferObject;
}


OpenGLFrameBuffer::OpenGLFrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: FrameBuffer(width, height, offscreen)
{
	if (mOffscreen)
		glGenFramebuffers(1, &mFrameBufferObject);
	else
		mFrameBufferObject = 0;
}

RcEngine::OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	DetachAll();

	if(mFrameBufferObject != 0)
		glDeleteFramebuffers(1, &mFrameBufferObject);
}

void OpenGLFrameBuffer::DoBind()
{
	BindFBO(mFrameBufferObject);

	if (mFrameBufferObject != 0 && mColorViews.size())
	{
		assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

		std::vector<GLenum> targets(mColorViews.size());
		for (size_t i = 0; i < mColorViews.size(); ++ i)
		{
			targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(static_cast<GLsizei>(targets.size()), &targets[0]);
	}

	OGL_ERROR_CHECK();
}

void OpenGLFrameBuffer::DoUnbind()
{
}

void OpenGLFrameBuffer::SwapBuffers()
{

}

void OpenGLFrameBuffer::Resize( uint32_t width, uint32_t height )
{
	if ((width != mWidth) || (height != mHeight))
	{
		mViewport.Width = width;
		mViewport.Height = height;
		mWidth = width;
		mHeight = height;
		mViewport.Top = 0;
		mViewport.Left = 0;
		mDirty = true;
	}
}

bool OpenGLFrameBuffer::CheckFramebufferStatus()
{
	OGL_ERROR_CHECK();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status == GL_FRAMEBUFFER_COMPLETE)
		return true;

	String errStr;
	switch (status)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		errStr = "The default framebuffer does not exist";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		errStr = "Combination of internal formats of the attached images violates an implementation-dependent set of restrictions";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		errStr = "Any of the framebuffer attachment points are framebuffer incomplete";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		errStr = "The framebuffer does not have at least one image attached to it";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :
		errStr = "Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target";
		break;
	default:
		break;
	}

	OGL_ERROR_CHECK();

	std::cout << errStr << std::endl;
	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "OpenGL FrameBuffer Error", "OpenGLFrameBuffer::CheckFramebufferStatus");
	return false;
}


}


