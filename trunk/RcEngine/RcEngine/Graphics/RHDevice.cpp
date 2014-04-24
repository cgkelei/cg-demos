#include <Graphics/RHDevice.h>
#include <Graphics/RHFactory.h>
#include <Graphics/RHFrameBuffer.h>
#include <Graphics/RHResource.h>
#include <Graphics/RHOperation.h>
#include <Core/Environment.h>

namespace RcEngine {

RHDevice::RHDevice(  )
	: mRHFactory(nullptr),
	  mCurrentFrontStencilRef(0),
	  mCurrentBackStencilRef(0),
	  mCurrentBlendFactor(ColorRGBA::Black),
	  mCurrentSampleMask(0)
{


	Environment::GetSingleton().mRHDevice = this;
}

RHDevice::~RHDevice( void )
{
	SAFE_DELETE(mRHFactory);
}

void RHDevice::BindFrameBuffer( const shared_ptr<RHFrameBuffer>& fb )
{
	if (mCurrentFrameBuffer != fb)
	{
		if (mCurrentFrameBuffer)
			mCurrentFrameBuffer->OnUnbind();

		mCurrentFrameBuffer = fb;
	}
	
	if(mCurrentFrameBuffer->IsDirty())
	{
		// update FBO info
		mCurrentFrameBuffer->OnBind();

		SetViewport(mCurrentFrameBuffer->GetViewport());
	}
}

void RHDevice::Draw( const EffectTechnique* technique, const RHOperation& operation )
{

}

void RHDevice::BindShaderPipeline( const shared_ptr<RHShaderPipeline>& pipeline )
{
	if (mCurrentShaderPipeline != pipeline)
	{
		mCurrentShaderPipeline->OnUnbind();
		
		mCurrentShaderPipeline = pipeline;
		mCurrentShaderPipeline->OnBind();
	}
}


}