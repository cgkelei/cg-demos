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
#include <Graphics/RenderPath.h>
#include <Graphics/CameraController1.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Scene/Light.h>
#include <Math/MathUtil.h>
#include <Graphics/Image.h>


using namespace RcEngine;

class MeshTestApp : public Application
{
public:
	MeshTestApp(const String& config)
		: Application(config)
	{

	}

	virtual ~MeshTestApp(void)
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

		mCamera = device->GetScreenFrameBuffer()->GetCamera();

		//mRenderPath = std::make_shared<ForwardPath>();
		
		mRenderPath = std::make_shared<DeferredPath>();
		mRenderPath->OnGraphicsInit();

		/*Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		spotLight->SetDirection(float3(0, -1.5, -1));
		spotLight->SetLightColor(float3(1, 1, 1));
		spotLight->SetRange(300.0);
		spotLight->SetPosition(float3(0.0f, 150.0f, 100.0f));
		spotLight->SetAttenuation(1.0f, 0.0f);
		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(60));
		spotLight->SetCastShadow(true);
		spotLight->SetSpotlightNearClip(10);
		sceneMan.GetRootSceneNode()->AttachObject(spotLight);*/
	}


	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		Light* mDirLight = sceneMan->CreateLight("Sun", LT_DirectionalLight);
		mDirLight->SetDirection(float3(0, -8, -1));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetCastShadow(false);
		sceneMan->GetRootSceneNode()->AttachObject(mDirLight);

		Entity* entity;
		SceneNode* sceneNode;

		//mCamera->CreateLookAt(float3(213.4, 405.5, -30.5),  float3(214.3, 405.1, -30.6), float3(0.4, 0.9, -0.0));
		//mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		//Entity* sponzaEntity = sceneMan->CreateEntity("Sponza", "Sponza.mesh",  "Custom");
		//SceneNode* sponzaNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sponza");
		//sponzaNode->SetPosition(float3(0, 0, 0));
		//sponzaNode->SetScale(0.45f);
		//sponzaNode->AttachObject(sponzaEntity);

		mCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 1000.0f );

		entity = sceneMan->CreateEntity("Ground", "./Geo/Ground.mesh",  "Custom");
		sceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Ground");
		sceneNode->SetScale(float3(2.5,2.5,2.5));
		sceneNode->SetPosition(float3(0, 0, 0));
		sceneNode->AttachObject(entity);

		entity = sceneMan->CreateEntity("Nanosuit", "./Nanosuit/Nanosuit.mesh",  "Custom");
		sceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Nanosuit");
		sceneNode->SetScale(float3(2,2,2));
		sceneNode->SetPosition(float3(-50,0,0));
		sceneNode->AttachObject(entity);

		//entity = sceneMan->CreateEntity("Dude", "./Dude/dude.mesh",  "Custom");
		//sceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Dude");
		//sceneNode->SetScale(float3(0.5, 0.5, 0.5));
		//sceneNode->SetPosition(float3(50,0,0));
		//sceneNode->SetWorldRotation(QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::PI));
		//sceneNode->AttachObject(entity);

		//AnimationPlayer* animPlayer = entity->GetAnimationPlayer();
		//AnimationState* takeClip = animPlayer->GetClip("Take 001");
		//takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
		//animPlayer->PlayClip("Take 001");

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);
		mCameraControler->SetMoveSpeed(100.0f);
		mCameraControler->SetMoveInertia(true);
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
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		mRenderPath->RenderScene();

		device->BindFrameBuffer(device->GetScreenFrameBuffer());
		device->GetScreenFrameBuffer()->SwapBuffers();
	}

	void CalculateFrameRate()
	{

	}

	void WindowResize(uint32_t width, uint32_t height)
	{
		mRenderPath->OnWindowResize(width, height);
	}

protected:
	int mFramePerSecond;

	shared_ptr<Effect> mFSQuadEffect;
	shared_ptr<Texture> mTexture;
	RenderOperation mFSQuad;
	shared_ptr<Camera> mCamera;
	
	shared_ptr<RenderPath> mRenderPath;

	Test::FPSCameraControler* mCameraControler;
};


int main()
{
	//Image img;
	//img.LoadFromDDS("E:/Engines/RcEngine/Media/Mesh/Geo/sand_diffuse.dds");


	MeshTestApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}