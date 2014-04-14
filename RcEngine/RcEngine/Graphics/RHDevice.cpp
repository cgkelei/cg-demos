#include <Graphics/RHDevice.h>
#include <Graphics/RHFactory.h>
#include <Graphics/RHFrameBuffer.h>
#include <Graphics/RHResource.h>

namespace RcEngine {

RHDevice::RHDevice( const RenderSettings& settings )
	: mRenderSettings(settings),
	  mRenderFactory(nullptr),
	  mCurrentFrontStencilRef(0),
	  mCurrentBackStencilRef(0),
	  mCurrentBlendFactor(ColorRGBA::Black),
	  mCurrentSampleMask(0)
{

}

RHDevice::~RHDevice( void )
{
	SAFE_DELETE(mRenderFactory);
}

void RHDevice::BindFrameBuffer( const shared_ptr<RHFrameBuffer>& fb )
{
	if( mCurrentFrameBuffer && (fb != mCurrentFrameBuffer) )
	{	
		mCurrentFrameBuffer->OnUnbind();
	}

	mCurrentFrameBuffer = fb; 

	if(mCurrentFrameBuffer->IsDirty())
	{
		// update FBO info
		mCurrentFrameBuffer->OnBind();
	}

	// this will update viewport info
	DoBindFrameBuffer(mCurrentFrameBuffer);
}

void RHDevice::Draw( const RHOperation& operation )
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