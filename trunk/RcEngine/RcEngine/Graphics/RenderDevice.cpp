#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Core/Environment.h>

namespace RcEngine {

RenderDevice::RenderDevice(  )
	: mRHFactory(nullptr),
	  mCurrentFrontStencilRef(0),
	  mCurrentBackStencilRef(0),
	  mCurrentBlendFactor(ColorRGBA::Black),
	  mCurrentSampleMask(0)
{


	Environment::GetSingleton().mRenderDevice = this;
}

RenderDevice::~RenderDevice( void )
{
	SAFE_DELETE(mRHFactory);
}

void RenderDevice::BindFrameBuffer( const shared_ptr<FrameBuffer>& fb )
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

void RenderDevice::Draw( const EffectTechnique* technique, const RenderOperation& operation )
{

}

void RenderDevice::BindShaderPipeline( const shared_ptr<ShaderPipeline>& pipeline )
{
	if (mCurrentShaderPipeline != pipeline)
	{
		mCurrentShaderPipeline->OnUnbind();
		
		mCurrentShaderPipeline = pipeline;
		mCurrentShaderPipeline->OnBind();
	}
}


}