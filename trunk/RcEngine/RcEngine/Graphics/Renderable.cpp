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
	: mRenderOperation( new RenderOperation )
{

}

Renderable::~Renderable()
{
			
}

EffectTechnique* Renderable::GetTechnique() const
{
	return GetMaterial()->GetCurrentTechnique();
}

void Renderable::AddToRenderQueue()
{

}


void Renderable::Render()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	EffectTechnique* technique = GetTechnique();
			
	OnRenderBegin();
	device.Render(*technique, *GetRenderOperation());
	OnRenderEnd();	
}

void Renderable::OnRenderBegin()
{
	// Get world transforms
	uint32_t matCounts = GetWorldTransformsCount();
	Matrix4f* matWorlds = new Matrix4f[matCounts];
	GetWorldTransforms(matWorlds);

	// Get material 
	shared_ptr<Material> material = GetMaterial();

	// Set world transform
	EffectParameter* world = material->GetCustomParameter(EPU_WorldMatrix)->EffectParam;
	if (world)
	{
		world->SetValue(matWorlds[0]);
	}

				
	// Setup other material parameter
	material->ApplyMaterial();

	delete[] matWorlds;
}

void Renderable::OnRenderEnd()
{

}


} // Namespace RcEngine