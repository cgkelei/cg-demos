#include <Graphics/EffectPass.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/ShaderProgram.h>
#include <Core/Context.h>

namespace RcEngine {

EffectPass::EffectPass()
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

} // Namespace RcEngine