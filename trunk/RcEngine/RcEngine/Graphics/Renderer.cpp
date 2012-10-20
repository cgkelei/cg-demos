#include <Graphics/Renderer.h>

namespace RcEngine {

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::DrawFSQuad( const shared_ptr<Material>& mat, const String& tech )
{

}

void Renderer::DrawGeometry( const String& tech, const  String& matClass, RenderOrder order )
{

}

void Renderer::DrawRenderable( Camera* camera, const String& tech, const  String& matClass, RenderOrder order )
{

}

void Renderer::RenderScene( const shared_ptr<Pipeline>& pipeline )
{
	
}

void Renderer::UpdateShadowMap()
{

}

}