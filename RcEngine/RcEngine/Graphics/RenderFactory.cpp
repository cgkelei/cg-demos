#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/VertexDeclaration.h>
#include <IO/FileStream.h>
#include <Core/Exception.h>

namespace RcEngine {


RenderFactory::RenderFactory(void)
{
}


RenderFactory::~RenderFactory(void)
{

}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return std::make_shared<VertexDeclaration>(elems, count);
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( const std::vector<VertexElement>& elems )
{
	return std::make_shared<VertexDeclaration>(elems);
}

shared_ptr<DepthStencilState> RenderFactory::CreateDepthStencilState( const DepthStencilStateDesc& desc )
{
	shared_ptr<DepthStencilState> retVal;

	auto found = mDepthStecilStatePool.find(desc);
	if (found == mDepthStecilStatePool.end())
	{
		retVal = CreateDepthStencilStateImpl(desc);
		mDepthStecilStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}
	

shared_ptr<BlendState> RenderFactory::CreateBlendState( const BlendStateDesc& desc )
{
	shared_ptr<BlendState> retVal;

	auto found = mBlendStatePool.find(desc);
	if (found == mBlendStatePool.end())
	{
		retVal = CreateBlendStateImpl(desc);
		mBlendStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}


shared_ptr<RasterizerState> RenderFactory::CreateRasterizerState( const RasterizerStateDesc& desc )
{
	shared_ptr<RasterizerState> retVal;

	auto found = mRasterizerStatePool.find(desc);
	if (found == mRasterizerStatePool.end())
	{
		retVal = CreateRasterizerStateImpl(desc);
		mRasterizerStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}

shared_ptr<SamplerState> RenderFactory::CreateSamplerState( const SamplerStateDesc& desc )
{
	shared_ptr<SamplerState> retVal;

	auto found = mSamplerStatePool.find(desc);
	if (found == mSamplerStatePool.end())
	{
		retVal = CreateSamplerStateImpl(desc);
		mSamplerStatePool.insert(std::make_pair(desc,retVal));
	}
	else
	{
		retVal = found->second;
	}
	return retVal;
}

shared_ptr<Material> RenderFactory::CreateMaterialFromFile( const String& matName, const String& path )
{
	MaterialMapIter find = mMaterialPool.find(matName);
	if ( find == mMaterialPool.end())
	{
		FileStream file;
		if (!file.Open(path, FILE_READ))
		{
			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, 
				"Error: " + path + " not exits!", "RenderFactory::CreateMaterialFromFile");
		}
		mMaterialPool[matName] = Material::LoadFrom(file);

		//shared_ptr<Material> m = mMaterialPool[matName]->Clone();
	}		
	return mMaterialPool[matName];
}

shared_ptr<Effect> RenderFactory::CreateEffectFromFile(const String& effectName, const String& effectFile)
{
	auto found = mEffectPool.find(effectName);
	if ( found == mEffectPool.end())
	{
		FileStream file;
		if (!file.Open(effectFile, FILE_READ))
		{
			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, 
				"Error: " + effectFile + " not exits!", "RenderFactory::CreateMaterialFromFile");
		}

		mEffectPool[effectName] = Effect::LoadFrom(file);
	}	

	return mEffectPool[effectName];
}

} // Namespace RcEngine
