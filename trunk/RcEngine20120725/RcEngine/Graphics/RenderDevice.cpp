#include "Graphics/RenderDevice.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Camera.h"
#include "Graphics/RenderOperation.h"
#include "Core/Context.h"

namespace RcEngine {
	namespace Render {

		RenderDevice::RenderDevice(void)
			: mRenderFactory(0), mCurrentFrameBuffer(0), mScreenFrameBuffer(0)
		{
			Core::Context::GetSingleton().SetRenderDevice(this);
		}

		RenderDevice::~RenderDevice(void)
		{
		}

		void RenderDevice::Resize( uint32_t width, uint32_t height )
		{
			Camera* cam = GetCurrentFrameBuffer()->GetCamera();
			cam->SetProjectionParams(cam->GetFov(), (float)width/(float)height, cam->GetNearPlane(), cam->GetFarPlane());
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