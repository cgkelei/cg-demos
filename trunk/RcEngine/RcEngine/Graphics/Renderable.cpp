#include <Graphics/Renderable.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Texture.h>
#include <Core/Context.h>

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
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	shared_ptr<Material> mat = GetMaterial();
	EffectTechnique* technique = GetTechnique();
			
	OnRenderBegin();
	device.Render(*technique, *GetRenderOperation());
	OnRenderEnd();	
}

void Renderable::OnRenderBegin()
{
	// Get world transforms
	uint32_t matCounts = GetWorldTransformsCount();
	vector<Matrix4f> matWorlds(matCounts);
	GetWorldTransforms(&matWorlds[0]);

	// Get material 
	shared_ptr<Material> material = GetMaterial();

	// Set world transform
	MaterialParameter* world = material->GetCustomParameter(EPU_WorldMatrix);
	
	if (world)
	{
		if (matCounts == 1)
		{
			world->EffectParam->SetValue(matWorlds[0]);
		}
		else
		{
			world->EffectParam->SetValue(matWorlds);
		}	
	}

				
	// Setup other material parameter
	material->ApplyMaterial();
}

void Renderable::OnRenderEnd()
{

}


} // Namespace RcEngine