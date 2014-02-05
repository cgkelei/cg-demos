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
	: mRenderFactory(nullptr),
	  mCurrentFrameBuffer(nullptr),
	  mScreenFrameBuffer(nullptr),
	  mCurrentBlendState(nullptr),
	  mCurrentRasterizerState(nullptr), 
	  mCurrentDepthStencilState(nullptr),
	  mCurrentFrontStencilRef(0),
	  mCurrentBackStencilRef(0),
	  mCurrentBlendFactor(ColorRGBA::Black),
	  mCurrentSampleMask(0)
{
	Context::GetSingleton().SetRenderDevice(this);
}

RenderDevice::~RenderDevice(void)
{
}

void RenderDevice::Resize( uint32_t width, uint32_t height )
{
	mScreenFrameBuffer->Resize(width, height);
}


void RenderDevice::BindFrameBuffer( const shared_ptr<FrameBuffer>& fb )
{
	assert(fb);

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

void RenderDevice::Render( EffectTechnique& tech, RenderOperation& op )
{
	DoRender(tech, op);
}

} // Namespace RcEngine