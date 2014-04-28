#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <MainApp/Application.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Core/Utility.h>

namespace RcEngine {

RenderFactory::RenderFactory()
{
	Environment::GetSingleton().mRenderFactory = this;
}

shared_ptr<BlendState> RenderFactory::CreateBlendState( const BlendStateDesc& desc )
{
	if (mBlendStatePool.find(desc) == mBlendStatePool.end())
	{
		mBlendStatePool[desc] = CreateBlendStateImpl(desc);
	}

	return mBlendStatePool[desc];
}

shared_ptr<SamplerState> RenderFactory::CreateSamplerState( const SamplerStateDesc& desc )
{
	if (mSamplerStatePool.find(desc) == mSamplerStatePool.end())
	{
		mSamplerStatePool[desc] = CreateSamplerState(desc);
	}

	return mSamplerStatePool[desc];
}

shared_ptr<RasterizerState> RenderFactory::CreateRasterizerState( const RasterizerStateDesc& desc )
{
	if (mRasterizerStatePool.find(desc) == mRasterizerStatePool.end())
	{
		mRasterizerStatePool[desc] = CreateRasterizerStateImpl(desc);
	}

	return mRasterizerStatePool[desc];
}

shared_ptr<DepthStencilState> RenderFactory::CreateDepthStencilState( const DepthStencilStateDesc& desc )
{
	if (mDepthStecilStatePool.find(desc) == mDepthStecilStatePool.end())
	{
		mDepthStecilStatePool[desc] = CreateDepthStencilStateImpl(desc);
	}

	return mDepthStecilStatePool[desc];
}

shared_ptr<Shader> RenderFactory::LoadShaderFromFile( ShaderType shaderType, const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint /*= ""*/ )
{
	size_t shaderSeed = 0;

	HashCombine(shaderSeed, filename);
	HashCombine(shaderSeed, entryPoint);
	for (uint32_t i = 0; i < macroCount; ++i)
	{
		HashCombine(shaderSeed, macros->Name);
		HashCombine(shaderSeed, macros->Definition);
	}

	if (mShaderPool.find(shaderSeed) == mShaderPool.end())
	{
		shared_ptr<Shader> shader = CreateShader(shaderType);
		
		String shaderFile;
		if ( Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			shaderFile = "HLSL/" + filename + ".hlsl";
		}
		else 
		{
			shaderFile = "GLSL/" + filename + ".glsl";
		}

		shader->LoadFromFile(FileSystem::GetSingleton().Locate(shaderFile), macros, macroCount, entryPoint);

		mShaderPool[shaderSeed] = shader;
	}

	return mShaderPool[shaderSeed];
}

shared_ptr<Texture> RenderFactory::LoadTextureFromFile( const String& filename )
{
	shared_ptr<Texture> tex;
	return tex;
}

}