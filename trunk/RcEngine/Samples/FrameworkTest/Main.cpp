#include <MainApp/Application.h>
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

		mCamera = device->GetScreenFrameBuffer()->GetCamera();

		mCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 1000.0f );

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(100.0f);
		mCameraControler->SetMoveInertia(true);
	}


	void LoadContent()
	{
		mFSQuad.PrimitiveType = PT_Triangle_List;
		mFSQuad.SetVertexRange(0, 3);

		ResourceManager& resMan = ResourceManager::GetSingleton();
		auto textureRes = resMan.GetResourceByName<TextureResource>(RT_Texture, "./Geo/sand_diffuse.dds", "Custom");
		mTexture = textureRes->GetTexture();
	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		mCameraControler->Update(deltaTime);
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();

		shared_ptr<FrameBuffer> screenFrameBuffer = device->GetScreenFrameBuffer();

		device->BindFrameBuffer(screenFrameBuffer);
		screenFrameBuffer->Clear(CF_Color | CF_Depth, ColorRGBA(0, 0, 0, 1), 1.0f, 0);

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
	shared_ptr<Camera> mCamera;
	Test::FPSCameraControler* mCameraControler;
};


int main()
{
	FrameApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}