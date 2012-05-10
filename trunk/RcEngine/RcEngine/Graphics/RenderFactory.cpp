#include "Graphics/RenderFactory.h"
#include "Graphics/Material.h"
#include "Content/MaterialContentLoader.h"

namespace RcEngine {
namespace Render {

RenderFactory::RenderFactory(void)
{
}


RenderFactory::~RenderFactory(void)
{
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexDeclarationDesc* inputElementDescs, uint32_t numElements )
{
	shared_ptr<VertexDeclaration> vd = std::make_shared<VertexDeclaration>();
	for(uint32_t i = 0; i < numElements; i++)
	{
		vd->AddElement(inputElementDescs[i].Offset, inputElementDescs[i].Format, inputElementDescs[i].Usage, inputElementDescs[i].UsageIndex);
	}
	return vd;
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( std::vector<VertexElement> elems )
{
	shared_ptr<VertexDeclaration> vd = std::make_shared<VertexDeclaration>();
	for (size_t i = 0; i < elems.size(); ++i)
	{
		vd->AddElement(elems[i]);
	}
	return vd;
}

shared_ptr<Material> RenderFactory::CreateMaterialFromFile( const String& matName, const String& path )
{
	Content::MaterialContentLoader loader(path);
	return Material::LoadFrom(&loader);
}


} // RenderSystem
} // RcEngine
