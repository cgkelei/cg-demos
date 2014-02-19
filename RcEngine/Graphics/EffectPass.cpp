#include <Graphics/EffectPass.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/ShaderProgram.h>
#include <Core/Context.h>

namespace RcEngine {

EffectPass::EffectPass( )
	: mValid(false),
	  mFrontStencilRef(0),
	  mBackStencilRef(0),
	  mBlendColor(0, 0, 0, 0),
	  mSampleMask(0)
{

}

EffectPass::~EffectPass()
{

}

void EffectPass::BeginPass()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	
	device.SetDepthStencilState(mDepthStencilState, mFrontStencilRef, mBackStencilRef);
	device.SetBlendState(mBlendState, mBlendColor, mSampleMask);
	device.SetRasterizerState(mRasterizerState);
	mShaderProgram->Bind();
}

void EffectPass::EndPass()
{
	mShaderProgram->Unbind();
}

EffectPass* EffectPass::Clone(Effect& effect)
{
	EffectPass* retVal = new EffectPass();


	retVal->mName =  mName;
	retVal->mValid = mValid;
	retVal->mBlendState = mBlendState;
	retVal->mDepthStencilState = mDepthStencilState;
	retVal->mRasterizerState = mRasterizerState;

	retVal->mFrontStencilRef = mFrontStencilRef;
	retVal->mBackStencilRef = mBackStencilRef;

	retVal->mBlendColor = mBlendColor;
	retVal->mSampleMask = mSampleMask;

	// clone shader program
	retVal->mShaderProgram = mShaderProgram->Clone(effect);

	return retVal;
}

} // Namespace RcEngine