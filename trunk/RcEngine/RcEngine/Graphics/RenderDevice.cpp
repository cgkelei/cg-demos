#include "Graphics/RenderDevice.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderOperation.h"
#include "Core/Context.h"

namespace RcEngine {
	namespace Render {

		RenderDevice::RenderDevice(void)
			: mRenderFactory(0), mCurrentFrameBuffer(0), mDefaultFrameBuffer(0)
		{
			Core::Context::GetSingleton().SetRenderDevice(this);
		}

		RenderDevice::~RenderDevice(void)
		{
		}

		void RenderDevice::Resize( uint32_t width, uint32_t height )
		{

		}

		void RenderDevice::BindFrameBuffer( FrameBuffer* fb )
		{
			assert(fb != NULL);

			if( mCurrentFrameBuffer && (fb != mCurrentFrameBuffer) )
			{	
				mCurrentFrameBuffer->OnUnbind();
			}

			mCurrentFrameBuffer = fb; 

			if(mCurrentFrameBuffer->IsDirty())
			{
				mCurrentFrameBuffer->OnBind();
				DoBindFrameBuffer(mCurrentFrameBuffer);
			}
		}

		FrameBuffer* RenderDevice::GetCurrentFrameBuffer()
		{
			return mCurrentFrameBuffer;
		}

		FrameBuffer* RenderDevice::GetDefaultFrameBuffer()
		{
			return mDefaultFrameBuffer;
		}

		RenderFactory* RenderDevice::GetRenderFactory() const
		{
			return mRenderFactory;
		}

		void RenderDevice::Render( EffectTechnique& tech, RenderOperation& op )
		{
			DoRender(tech, op);
		}

		


	} // RenderSystem
} // RcEngine