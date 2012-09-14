#include <Graphics/RenderDevice.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/BlendState.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/SamplerState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/FrameBuffer.h>
#include <Core/Context.h>

namespace RcEngine {


	RenderDevice::RenderDevice(void)
		: mRenderFactory(0), mCurrentFrameBuffer(0), mScreenFrameBuffer(0)
	{
		Context::GetSingleton().SetRenderDevice(this);
	}

	RenderDevice::~RenderDevice(void)
	{
	}

	void RenderDevice::Resize( uint32_t width, uint32_t height )
	{
		mScreenFrameBuffer->Resize(width, height);

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

} // Namespace RcEngine