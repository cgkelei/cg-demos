#include "OpenGLRenderView.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderDevice.h"
#include "OpenGLTexture.h"
#include "Core/Exception.h"
#include "Core/Context.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLRenderTarget2DView::OpenGLRenderTarget2DView( Texture* tex2d, unsigned int arrIndex, unsigned int level )
			: mArrIndex(arrIndex), mLevel(level)
		{
			if(tex2d->GetTextureType() != TT_Texture2D)
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Type Error, Only 2D Texture Needed",
					"OpenGLRenderTarget2DView::OpenGLRenderTarget2DView");
			}

			mTexture = static_cast<OpenGLTexture2D*>(tex2d);
			mWidth = mTexture->GetWidth(level);
			mHeight = mTexture->GetHeight(level);
			mFormat = mTexture->GetTextureFormat();
		}

		OpenGLRenderTarget2DView::~OpenGLRenderTarget2DView()
		{

		}

		void OpenGLRenderTarget2DView::ClearColor( const Math::ColorRGBA& clr )
		{

		}

		void OpenGLRenderTarget2DView::OnAttach( FrameBuffer* fb, unsigned int att )
		{
			unsigned int index = att - ATT_Color0;
			mAttachedFrameBuffer = fb;

			OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Core::Context::GetSingleton().GetRenderDevicePtr());

			FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();

			device->BindFrameBuffer(mAttachedFrameBuffer);

			if(mTexture->GetOpenGLTextureTarget() == GL_TEXTURE_2D)
			{		

				if(mTexture->GetSampleCount() <= 1)
				{
			
					glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT + index, GL_TEXTURE_2D, mTexture->GetOpenGLTexture(), mLevel);
				}
				else
				{
					glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
						GL_COLOR_ATTACHMENT0_EXT + index, GL_RENDERBUFFER_EXT, mTexture->GetOpenGLTexture());
				}
			}
			else
			{
				// 2D Texture Array
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
					mTexture->GetOpenGLTexture(), mArrIndex, mLevel);
			}

			device->BindFrameBuffer(oldFrameBuffer);
		}

		void OpenGLRenderTarget2DView::OnDetach( FrameBuffer* fb, unsigned int att )
		{
			unsigned int index = att - ATT_Color0;
			mAttachedFrameBuffer = fb;

			OpenGLRenderDevice* device = static_cast<OpenGLRenderDevice*>(Core::Context::GetSingleton().GetRenderDevicePtr());

			FrameBuffer* oldFrameBuffer = device->GetCurrentFrameBuffer();

			device->BindFrameBuffer(mAttachedFrameBuffer);

			if(mTexture->GetOpenGLTextureTarget() == GL_TEXTURE_2D)
			{		

				if(mTexture->GetSampleCount() <= 1)
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

			device->BindFrameBuffer(oldFrameBuffer);

			mAttachedFrameBuffer = NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		OpenGLScreenRenderTarget2DView::OpenGLScreenRenderTarget2DView( unsigned int width, unsigned int height, PixelFormat fmt )
		{
			mWidth = width;
			mHeight = height;
			mFormat = fmt;
		}

		OpenGLScreenRenderTarget2DView::~OpenGLScreenRenderTarget2DView()
		{

		}

		void OpenGLScreenRenderTarget2DView::OnAttach( FrameBuffer* fb, unsigned int index )
		{
			OpenGLFrameBuffer* oglFrameBuffer = static_cast<OpenGLFrameBuffer*>(fb);
			if(oglFrameBuffer->GetFrameBufferObject() != 0)
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "ScreenRenderTarget2DView Can Only Attach To Screen Frame Buffer",
					"OpenGLScreenRenderTarget2DView::OnAttached");
			}
			mAttachedFrameBuffer = fb;
		}

		void OpenGLScreenRenderTarget2DView::OnDetach( FrameBuffer* fb, unsigned int index )
		{
			OpenGLFrameBuffer* oglFrameBuffer = static_cast<OpenGLFrameBuffer*>(fb);
			if(oglFrameBuffer->GetFrameBufferObject() != 0)
			{
				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "OpenGLScreenRenderTarget2DView Can Only Attach To Screen Frame Buffer",
					"OpenGLScreenRenderTarget2DView::OnAttached");
			}
			mAttachedFrameBuffer = fb;
		}

	}
}
