#include "Graphics/RenderDevice.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderOperation.h"

namespace RcEngine {
	namespace RenderSystem {

		RenderDevice::RenderDevice(void)
			: mRenderFactory(0)
		{

		}

		RenderDevice::~RenderDevice(void)
		{
		}

		void RenderDevice::BeginFrame()
		{

		}

		void RenderDevice::EndFrame()
		{

		}

		void RenderDevice::SwapBuffers()
		{

		}

		void RenderDevice::BindFrameBuffer( FrameBuffer* fb )
		{
			assert(fb != NULL);

			if(fb != mCurrentFrameBuffer)
			{
				if(mCurrentFrameBuffer)
				{
					mCurrentFrameBuffer->OnUnbind();
					mCurrentFrameBuffer = fb;
				}
			}

			if(mCurrentFrameBuffer->IsDirty())
			{
				mCurrentFrameBuffer->OnBind();
				DoBindFrame(mCurrentFrameBuffer);
			}
		}


		void RenderDevice::Resize( unsigned int width, unsigned int height )
		{

		}

		FrameBuffer* RenderDevice::GetCurrentFrameBuffer()
		{
			return mCurrentFrameBuffer;
		}

		RenderFactory* RenderDevice::GetRenderFactory() const
		{
			return mRenderFactory;
		}


	} // RenderSystem
} // RcEngine