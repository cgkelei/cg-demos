#include <MainApp/Application.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Resource/ResourceManager.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <IO/FileSystem.h>
#include <Graphics/Effect.h>
#include <Core/Environment.h>

using namespace RcEngine;

class FrameApp : public Application
{
public:
	FrameApp(const String& config)
		: Application(config)
	{

	}

	virtual ~FrameApp(void)
	{

	}

protected:
	void Initialize()
	{
		ResourceManager& resMan = ResourceManager::GetSingleton();
		FileSystem& fileSys = FileSystem::GetSingleton();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		mFSQuadEffect = static_pointer_cast_checked<Effect>(
			resMan.GetResourceByName(RT_Effect, "FSQuad.effect.xml", "General") );

		resMan.GetResourceByName(RT_Effect, "Model.effect.xml", "General");

		auto textureRes = static_pointer_cast_checked<TextureResource>(
			resMan.GetResourceByName(RT_Texture, "Sword_2H_Frostmourne_D_01_Glow.dds", "General") );
		mTexture = textureRes->GetTexture();
	}


	void LoadContent()
	{
		//float4 vertices[] = { 
		//	float4(-1,  1, 0, 0),
		//	float4( 1,  1, 1, 0),
		//	float4(-1, -1, 0, 1),
		//	float4( 1, -1, 1, 1)
		//};

		//RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

		//ElementInitData initData;
		//initData.pData = vertices;
		//initData.rowPitch = sizeof(vertices);
		//auto vb = factory->CreateVertexBuffer(sizeof(vertices), EAH_GPU_Read, BufferCreate_Vertex, &initData);
		//mFSQuad.BindVertexStream(0, vb);

		//VertexElement ves[] = {
		//	VertexElement(0, VEF_Float2, VEU_Position),
		//	VertexElement(sizeof(float2), VEF_Float2, VEU_TextureCoordinate)
		//};
		//mFSQuad.VertexDecl = factory->CreateVertexDeclaration(ves, 2);

		//mFSQuad.PrimitiveType = PT_Triangle_Strip;
		//mFSQuad.SetVertexRange(0, 4);

		mFSQuad.PrimitiveType = PT_Triangle_List;
		mFSQuad.SetVertexRange(0, 3);

		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();

		//shared_ptr<Material> material = resMan.GetResourceByName<Material>(RT_Material, "Model.material.xml", "Custom");
			
	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{

	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();

		shared_ptr<FrameBuffer> screenFrameBuffer = device->GetScreenFrameBuffer();

		device->BindFrameBuffer(screenFrameBuffer);
		screenFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA(1, 0, 0, 1), 1.0f, 0);

		auto effectParam = mFSQuadEffect->GetParameterByName("ColorMap");
		effectParam->SetValue(mTexture->GetShaderResourceView());
		device->Draw(mFSQuadEffect->GetCurrentTechnique(), mFSQuad);


		screenFrameBuffer->SwapBuffers();
	}

	void CalculateFrameRate()
	{

	}

	void WindowResize(uint32_t width, uint32_t height)
	{

	}

protected:
	int mFramePerSecond;

	shared_ptr<Effect> mFSQuadEffect;
	shared_ptr<Texture> mTexture;
	RenderOperation mFSQuad;
};


int main()
{
	FrameApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}