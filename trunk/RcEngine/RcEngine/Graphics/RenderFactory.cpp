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


} // RenderSystem
} // RcEngine
