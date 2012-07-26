#include "Graphics/RenderFactory.h"
#include "Graphics/Material.h"
#include "Content/MaterialContentLoader.h"
#include "IO/FileStream.h"
#include "Core/Exception.h"

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
	MaterialMapIter find = mMaterialPool.find(matName);
	if ( find == mMaterialPool.end())
	{
		FileStream file;
		if (!file.Open(path, FILE_READ))
		{
			ENGINE_EXCEPT(Core::Exception::ERR_FILE_NOT_FOUND, 
				"Error: " + path + " not exits!", "RenderFactory::CreateMaterialFromFile");
		}

		mMaterialPool[matName] = Material::LoadFrom(file);
	}	
	return mMaterialPool[matName];
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
