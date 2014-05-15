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
#include <Core/Profiler.h>

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

		factory->LoadShaderFromFile(ST_Compute, "TiledDeferredShading", NULL, 0, "TiledDeferredCSMain");


		mCamera = std::make_shared<Camera>();

		mRenderPath = std::make_shared<DeferredPath>();
		//mRenderPath = std::make_shared<ForwardPath>();
		mRenderPath->OnGraphicsInit(mCamera);
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		Entity* entity;
		SceneNode* sceneNode;

		mCamera->CreateLookAt(float3(-307.441010, 141.725555, -15.163310), float3(-308.427032, 141.578644, -15.241874), float3(-0.146448, 0.989150, -0.011668));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		Entity* sponzaEntity = sceneMan->CreateEntity("Sponza", "Sponza.mesh",  "Custom");
		SceneNode* sponzaNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sponza");
		sponzaNode->SetPosition(float3(0, 0, 0));
		sponzaNode->SetScale(0.45f);
		sponzaNode->AttachObject(sponzaEntity);

		//mCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
		//mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );


		/*std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
		start = std::chrono::high_resolution_clock::now();

		ENGINE_PUSH_CPU_PROFIER("Load Texture");

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

		ENGINE_POP_CPU_PROFIER("Load Texture");
		end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed_seconds = end-start;
		std::time_t end_time = std::chrono::high_resolution_clock::to_time_t(end);
		std::cout << "finished computation at " << std::ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s\n";

		ENGINE_DUMP_PROFILERS();*/

		//ENGINE_PUSH_CPU_PROFIER("test");
		//ENGINE_DUMP_PROFILERS();

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

		// Set as default camera
		auto screenFB = Environment::GetSingleton().GetRenderDevice()->GetScreenFrameBuffer();
		screenFB->SetCamera(mCamera);

		SetupLights();
	}

	void SetupLights()
	{
		SceneManager& sceneMan = *Environment::GetSingleton().GetSceneManager();

		Light* mDirLight = sceneMan.CreateLight("Sun", LT_DirectionalLight);
		mDirLight->SetDirection(float3(00, -1, 0.5));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetCastShadow(false);
		sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

		/*Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		spotLight->SetDirection(float3(0, -1.5, -1));
		spotLight->SetLightColor(float3(1, 0, 1));
		spotLight->SetRange(300.0);
		spotLight->SetPosition(float3(0.0f, 150.0f, 100.0f));
		spotLight->SetAttenuation(1.0f, 0.0f);
		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(60));
		spotLight->SetCastShadow(false);
		spotLight->SetSpotlightNearClip(10);
		sceneMan.GetRootSceneNode()->AttachObject(spotLight);*/

		Light* mPointLight = sceneMan.CreateLight("Point", LT_PointLight);
		mPointLight->SetLightColor(float3(1, 1, 0));
		mPointLight->SetRange(80.0f);
		mPointLight->SetAttenuation(1.0f, 0.0f);
		mPointLight->SetCastShadow(false);
		mPointLight->SetPosition(float3(0, 0, 0));
		sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

		Light* mSpotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		mSpotLight->SetLightColor(float3(0, 1, 0));
		mSpotLight->SetRange(250.0f);
		mSpotLight->SetPosition(float3(-442, 80, -16));
		mSpotLight->SetDirection(float3(-1, 0, 0));
		mSpotLight->SetAttenuation(1.0f, 0.0f);
		mSpotLight->SetSpotAngle(Mathf::ToRadian(30), Mathf::ToRadian(40));
		sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);


		{
		float3 direction = Normalize(float3(-111.5f, 398.1f, 3.6f) - float3(-111.1, 380.1, 73.4));
		for (int i = 0; i < 4; ++i)
		{
		Light* spotLight = sceneMan.CreateLight("Spot" + std::to_string(i), LT_SpotLight);
		spotLight->SetLightColor(float3(1, 1, 0));
		spotLight->SetRange(150);
		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, 3.6f));
		spotLight->SetDirection(direction);
		spotLight->SetAttenuation(1.0f, 0.0f);
		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
		sceneMan.GetRootSceneNode()->AttachObject(spotLight);
		}

		direction = Normalize(float3(-111.5f, 398.1f, 35.7f) - float3(-111.1, 380.1, -111.3));
		for (int i = 0; i < 4; ++i)
		{
		Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		spotLight->SetLightColor(float3(0, 1, 1));
		spotLight->SetRange(150);
		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, -35.7f));
		spotLight->SetDirection(direction);
		spotLight->SetAttenuation(1.0f, 0.0f);
		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
		sceneMan.GetRootSceneNode()->AttachObject(spotLight);
		}
		}
	}

	void UnloadContent()
	{

	}

	void Update(float deltaTime)
	{
		CalculateFrameRate();
		mCameraControler->Update(deltaTime);

		if ( InputSystem::GetSingleton().KeyPress(KC_Q) )
		{
			auto target = mCamera->GetLookAt();
			auto eye = mCamera->GetPosition();
			auto up = mCamera->GetUp();
			
			FILE* f = fopen("E:/camera.txt", "w");
			fprintf(f, "float3(%f, %f, %f), float3(%f, %f, %f), float3(%f, %f, %f)",
				eye[0], eye[1], eye[2], 
				target[0], target[1], target[2],
				up[0], up[1], up[2]);
			fclose(f);
		}

		if ( InputSystem::GetSingleton().KeyPress(KC_Space) )
		{
			mRenderPath->mVisualLightsWireframe = !mRenderPath->mVisualLightsWireframe;
		}

		char buffer[255];
		std::sprintf(buffer, "FPS: %d", mFramePerSecond);
		mMainWindow->SetTitle(buffer);
	}

	void Render()
	{
		RenderDevice* device = Environment::GetSingleton().GetRenderDevice();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		mRenderPath->RenderScene();
	}

	void CalculateFrameRate()
	{
		static int frameCount = 0;
		static float baseTime = 0;

		frameCount++;

		if (mTimer.GetGameTime()-baseTime >= 1.0f)
		{
			mFramePerSecond = frameCount;
			frameCount = 0;
			baseTime += 1.0f;
		}
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
	
	shared_ptr<DeferredPath> mRenderPath;

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