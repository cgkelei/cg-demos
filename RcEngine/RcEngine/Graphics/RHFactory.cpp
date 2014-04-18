#include <Graphics/RHFactory.h>
#include <Graphics/RHResource.h>
#include <Core/Environment.h>

namespace RcEngine {

RHFactory::RHFactory()
{
	Environment::GetSingleton().mRHFactory = this;
}

shared_ptr<RHBlendState> RHFactory::CreateBlendState( const RHBlendStateDesc& desc )
{
	if (mBlendStatePool.find(desc) == mBlendStatePool.end())
	{
		mBlendStatePool[desc] = CreateBlendStateImpl(desc);
	}

	return mBlendStatePool[desc];
}

shared_ptr<RHSamplerState> RHFactory::CreateSamplerState( const RHSamplerStateDesc& desc )
{
	if (mSamplerStatePool.find(desc) == mSamplerStatePool.end())
	{
		mSamplerStatePool[desc] = CreateSamplerState(desc);
	}

	return mSamplerStatePool[desc];
}

shared_ptr<RHRasterizerState> RHFactory::CreateRasterizerState( const RHRasterizerStateDesc& desc )
{
	if (mRasterizerStatePool.find(desc) == mRasterizerStatePool.end())
	{
		mRasterizerStatePool[desc] = CreateRasterizerStateImpl(desc);
	}

	return mRasterizerStatePool[desc];
}

shared_ptr<RHDepthStencilState> RHFactory::CreateDepthStencilState( const RHDepthStencilStateDesc& desc )
{
	if (mDepthStecilStatePool.find(desc) == mDepthStecilStatePool.end())
	{
		mDepthStecilStatePool[desc] = CreateDepthStencilStateImpl(desc);
	}

	return mDepthStecilStatePool[desc];
}

shared_ptr<RHShader> RHFactory::LoadShaderFromFile( ShaderType shaderType, const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint /*= ""*/ )
{
	//mShaderPool
	shared_ptr<RHShader> shader = CreateShader(shaderType);

	// Todo: load binary code if exits
	shader->LoadFromFile(filename, macros, macroCount, entryPoint);

	return shader;
}

}