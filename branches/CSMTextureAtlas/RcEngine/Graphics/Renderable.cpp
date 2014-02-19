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
	// Get material 
	shared_ptr<Material> material = GetMaterial();

	// Get world transforms
	float4x4 worldMatrix;
	uint32_t matCounts = GetWorldTransformsCount();

	if (matCounts > 0)
	{
		vector<float4x4> matWorlds(matCounts);
		GetWorldTransforms(&matWorlds[0]);

		//Last matrix is world transform matrix, previous is skin matrices.
		worldMatrix = matWorlds.back();

		// Skin matrix
		if (matCounts > 1)
		{	
			EffectParameter* skinMatricesParam = material->GetCustomParameter("SkinMatrices");
			if (skinMatricesParam)
			{
				// delete last world matrix first
				matWorlds.pop_back();
				skinMatricesParam->SetValue(matWorlds);
			}
		}
	}
			
	// Setup other material parameter
	material->ApplyMaterial(worldMatrix);
}

void Renderable::OnRenderEnd()
{

}


} // Namespace RcEngine