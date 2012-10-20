#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLRenderView.h"
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

void OpenGLFrameBuffer::Clear( uint32_t flags, ColorRGBA& clr, float depth, uint32_t stencil )
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// frame buffer must binded before clear
	shared_ptr<FrameBuffer> currentFrameBuffer = Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer();
	assert( this == currentFrameBuffer.get());

	for (size_t i = 0; i < mColorViews.size(); ++i)
	{
		if (mColorViews[i])
		{
			mColorViews[i]->ClearColor(clr);
		}		
	}

	if (mDepthStencilView)
	{
		mDepthStencilView->ClearDepthStencil(depth, stencil);
	}
}

void OpenGLFrameBuffer::SwapBuffers()
{

}

void OpenGLFrameBuffer::Resize( uint32_t width, uint32_t height )
{

}

}


