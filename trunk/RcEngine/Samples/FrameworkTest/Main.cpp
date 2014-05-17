#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraController1.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <nvImage/include/nvImage.h>

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

		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		mFSQuadEffect = static_pointer_cast_checked<Effect>(
			resMan.GetResourceByName(RT_Effect, "FSQuad.effect.xml", "General") );

		mComputeEffect = static_pointer_cast_checked<Effect>(
			resMan.GetResourceByName(RT_Effect, "Compute.effect.xml", "General") );

		mOutput = factory->CreateTexture2D(
			mMainWindow->GetWidth(), 
			mMainWindow->GetHeight(), 
			PF_RGBA32F, 
			1, 1, 
			1, 0, 
			EAH_GPU_Read | EAH_GPU_Write, 
			TexCreate_UAV | TexCreate_ShaderResource,
			NULL);

		mOutputUAV = factory->CreateTexture2DUAV(mOutput, 0, 0, 1);
	}


	void LoadContent()
	{
		mFSQuad.PrimitiveType = PT_Triangle_List;
		mFSQuad.SetVertexRange(0, 3);

		ResourceManager& resMan = ResourceManager::GetSingleton();
		auto textureRes = resMan.GetResourceByName<TextureResource>(RT_Texture, "GBuffer1.dds", "General");
		mTexture = textureRes->GetTexture();
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

		mComputeEffect->GetParameterByName("Input")->SetValue(mTexture->GetShaderResourceView());
		mComputeEffect->GetParameterByName("Output")->SetValue(mOutputUAV);
		
		enum { GroupSize = 32 };
		uint32_t numGroupX = (mMainWindow->GetWidth() + GroupSize - 1) / GroupSize;
		uint32_t numGroupY = (mMainWindow->GetHeight() + GroupSize - 1) / GroupSize;
		
		EffectTechnique* computeTech = mComputeEffect->GetCurrentTechnique();
		device->DispatchCompute(computeTech, numGroupX, numGroupY, 1);

		//device->GetRenderFactory()->SaveTextureToFile("E:/text.pfm", mOutput);

		auto effectParam = mFSQuadEffect->GetParameterByName("ColorMap");
		effectParam->SetValue(mOutput->GetShaderResourceView());
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
	shared_ptr<Effect> mComputeEffect;
	
	shared_ptr<Texture> mOutput;
	shared_ptr<UnorderedAccessView> mOutputUAV;
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