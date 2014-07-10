#include <Graphics/Renderable.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/GraphicsResource.h>
#include <Core/Environment.h>

namespace RcEngine {

Renderable::Renderable()
{

}

Renderable::~Renderable()
{

}

EffectTechnique* Renderable::GetTechnique() const
{
	return GetMaterial()->GetCurrentTechnique();
}

void Renderable::Render()
{
	EffectTechnique* technique = GetTechnique();
			
	OnRenderBegin();
	Environment::GetSingleton().GetRenderDevice()->Draw(technique, *GetRenderOperation());
	OnRenderEnd();	
}

void Renderable::OnRenderBegin()
{
	// Get material 
	shared_ptr<Material> material = GetMaterial();

	float4x4 xWorld;
	float4x4* xPalettes = nullptr;
	uint32_t numPalette = 0;
	GetWorldTransforms(&xWorld, numPalette, xPalettes);

	if (numPalette > 0)
	{
		EffectParameter* skinMatricesParam = material->GetEffect()->GetParameterByName("SkinMatrices");
		if (skinMatricesParam)
			skinMatricesParam->SetValue(xPalettes, numPalette);
	}

	// Setup other material parameter
	material->ApplyMaterial(xWorld);
}

void Renderable::OnRenderEnd()
{

}


} // Namespace RcEngine