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

shared_ptr<Material> RenderFactory::CreateMaterialFromFile( const String& matName, const String& path )
{
	Content::MaterialContent loader(path);
	return Material::LoadFrom(&loader);
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return std::make_shared<VertexDeclaration>(elems, count);
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( const std::vector<VertexElement>& elems )
{
	return std::make_shared<VertexDeclaration>(elems);
}


} // RenderSystem
} // RcEngine
