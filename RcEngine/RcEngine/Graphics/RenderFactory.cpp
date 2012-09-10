#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
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
	return std::make_shared<DepthStencilState>(desc);
}

shared_ptr<BlendState> RenderFactory::CreateBlendState( const BlendStateDesc& desc )
{
	return std::make_shared<BlendState>(desc);
}

shared_ptr<RasterizerState> RenderFactory::CreateRasterizerState( const RasterizerStateDesc& desc )
{
	return std::make_shared<RasterizerState>(desc);
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

} // Namespace RcEngine
