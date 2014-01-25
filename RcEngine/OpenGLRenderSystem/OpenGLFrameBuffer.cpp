#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLRenderView.h"
#include <Core/Exception.h>
#include <Core/Context.h>

namespace RcEngine {

OpenGLFrameBuffer::OpenGLFrameBuffer( uint32_t width, uint32_t height, bool offscreen /*= true*/ )
	: FrameBuffer(width, height, offscreen)
{
	if (mOffscreen)
	{
		glGenFramebuffersEXT(1, &mFrameBufferObject);
	} 
	else
	{
		mFrameBufferObject = 0;
	}
}

RcEngine::OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	DetachAll();

	if(mFrameBufferObject != 0)
		glDeleteFramebuffersEXT(1, &mFrameBufferObject);
}

void OpenGLFrameBuffer::DoBind()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBufferObject);
	
	if (mFrameBufferObject != 0 && mColorViews.size())
	{
		assert(GL_FRAMEBUFFER_COMPLETE_EXT == glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

		std::vector<GLenum> targets(mColorViews.size());
		for (size_t i = 0; i < mColorViews.size(); ++ i)
		{
			targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0_EXT + i);
		}
		glDrawBuffers(static_cast<GLsizei>(targets.size()), &targets[0]);
	}
}

void OpenGLFrameBuffer::DoUnbind()
{

}


void OpenGLFrameBuffer::SwapBuffers()
{

}

void OpenGLFrameBuffer::Resize( uint32_t width, uint32_t height )
{

}

bool OpenGLFrameBuffer::CheckFramebufferStatus()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status == GL_FRAMEBUFFER_COMPLETE)
		return true;

	ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "OpenGL FrameBuffer Error", "OpenGLFrameBuffer::CheckFramebufferStatus");
	return false;
}

}


