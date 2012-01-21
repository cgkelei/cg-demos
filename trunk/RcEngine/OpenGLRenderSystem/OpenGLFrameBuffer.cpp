#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "Core/Context.h"

namespace RcEngine
{
	namespace Render
	{
		OpenGLFrameBuffer::OpenGLFrameBuffer( unsigned int width, unsigned int height, bool offscreen /*= true*/ )
			: FrameBuffer(width, height, offscreen)
		{
			mLeft = 0;
			mTop = 0;

			if (mOffscreen)
			{
				glGenFramebuffersEXT(1, &mFrameBufferObject);
			} 
			else
			{
				mFrameBufferObject = 0;
			}
		}

		RcEngine::Render::OpenGLFrameBuffer::~OpenGLFrameBuffer()
		{
			if(mFrameBufferObject != 0)
				glDeleteFramebuffersEXT(1, &mFrameBufferObject);
		}


		void OpenGLFrameBuffer::DoBind()
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBufferObject);
			assert(GL_FRAMEBUFFER_COMPLETE_EXT == glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT));

			if (mFrameBufferObject != 0)
			{
				std::vector<GLenum> targets(mColorViews.size());
				for (size_t i = 0; i < mColorViews.size(); ++ i)
				{
					targets[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0_EXT + i);
				}
				glDrawBuffers(static_cast<GLsizei>(targets.size()), &targets[0]);
			}
			else
			{
				GLenum targets[] = { GL_BACK };
				glDrawBuffers(1, &targets[0]);
			}
		}

		void OpenGLFrameBuffer::DoUnbind()
		{

		}

		void OpenGLFrameBuffer::Clear( unsigned int flags, Math::ColorRGBA& clr, float depth, unsigned int stencil )
		{
			RenderDevice* device = Core::Context::GetSingleton().GetRenderDevicePtr();
			//FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();

			device->BindFrameBuffer(this);

			GLbitfield ogl_flags = 0;
			if (flags & CF_Color)
			{
				ogl_flags |= GL_COLOR_BUFFER_BIT;
				glClearColor(clr.R(), clr.G(), clr.B(), clr.A());
			}
			if (flags & CF_Depth)
			{
				ogl_flags |= GL_DEPTH_BUFFER_BIT;
				glClearDepth(depth);
			}
			if (flags & CF_Stencil)
			{
				ogl_flags |= GL_STENCIL_BUFFER_BIT;
				glClearStencil(stencil);
			}

			glClear(ogl_flags);

			//device->BindFrameBuffer(oldFrameBuffer);
		}

		void OpenGLFrameBuffer::SwapBuffers()
		{

		}

	}
}


