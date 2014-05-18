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
#include "pfm.h"

using namespace RcEngine;

class MeshTestApp : public Application
{
public:
	MeshTestApp(const String& config)
		: Application(config)
	{
		//ValidateDepth();
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

		mCamera = std::make_shared<Camera>();
		
		mRenderPath = std::make_shared<TiledDeferredPath>();
		//mRenderPath = std::make_shared<DeferredPath>();
		//mRenderPath = std::make_shared<ForwardPath>();
		mRenderPath->OnGraphicsInit(mCamera);
	}

	void LoadContent()
	{
		FileSystem& fileSys = FileSystem::GetSingleton();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

		/*	mCamera->CreateLookAt(float3(276.089172, 98.567436, -19.411501), float3(277.086884, 98.505020, -19.385706), float3(0.062396, 0.998050, 0.001613));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 1000.0f );

		Entity* sponzaEntity = sceneMan->CreateEntity("Sponza", "Sponza.mesh",  "Custom");
		SceneNode* sponzaNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Sponza");
		sponzaNode->SetPosition(float3(0, 0, 0));
		sponzaNode->SetScale(0.45f);
		sponzaNode->AttachObject(sponzaEntity);*/

		//mCamera->CreateLookAt(float3(9.772805, 230.538498, 287.610077), float3(9.744802, 229.937454, 286.811340), float3(-0.021059, 0.799220, -0.600669));
		mCamera->CreateLookAt(float3(-162.300583, 239.233307, 206.451996), float3(-161.861069, 238.546051, 205.873627), float3(0.415819, 0.726417, -0.547186));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mAppSettings.Width / (float)mAppSettings.Height, 1.0f, 3000.0f );

		auto entity = sceneMan->CreateEntity("Ground", "./Geo/Ground.mesh",  "Custom");
		auto sceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Ground");
		sceneNode->SetScale(float3(2.5,2.5,2.5));
		sceneNode->SetPosition(float3(0, 0, 0));
		sceneNode->AttachObject(entity);

		entity = sceneMan->CreateEntity("Nanosuit", "./Nanosuit/Nanosuit.mesh",  "Custom");
		sceneNode = sceneMan->GetRootSceneNode()->CreateChildSceneNode("Nanosuit");
		sceneNode->SetScale(float3(5,5,5));
		sceneNode->SetPosition(float3(0,0,0));
		sceneNode->AttachObject(entity);

		/*std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
		start = std::chrono::high_resolution_clock::now();

		ENGINE_PUSH_CPU_PROFIER("Load Texture");

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

		//Light* mDirLight = sceneMan.CreateLight("Sun", LT_DirectionalLight);
		//mDirLight->SetDirection(float3(00, -1, 0.5));
		//mDirLight->SetLightColor(float3(1, 1, 1));
		//mDirLight->SetCastShadow(false);
		//sceneMan.GetRootSceneNode()->AttachObject(mDirLight);

		Light* mPointLight = sceneMan.CreateLight("Point", LT_PointLight);
		mPointLight->SetLightColor(float3(0, 1, 0));
		mPointLight->SetRange(100.0f);
		mPointLight->SetAttenuation(1.0f, 0.0f);
		mPointLight->SetCastShadow(false);
		mPointLight->SetPosition(float3(0, 50, 35));
		sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

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

//////////////////////////////////////////////////////////////////////////
		///Light* mPointLight = sceneMan.CreateLight("Point", LT_PointLight);
		//mPointLight->SetLightColor(float3(1.69, 1, 0));
		//mPointLight->SetRange(80.0f);
		//mPointLight->SetAttenuation(1.0f, 0.0f);
		////mPointLight->SetAttenuation(7.0f, 3.0f, 4.0f);
		//mPointLight->SetCastShadow(false);
		//mPointLight->SetPosition(float3(550, 81, -18));
		//sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

		//mPointLight = sceneMan.CreateLight("Point", LT_PointLight);
		//mPointLight->SetLightColor(float3(0.85, 1, 0.67));
		//mPointLight->SetRange(90.0f);
		//mPointLight->SetAttenuation(9.0f, 3.0f, 8.0f);
		//mPointLight->SetCastShadow(false);
		//mPointLight->SetPosition(float3(105, 59, -48));
		//sceneMan.GetRootSceneNode()->AttachObject(mPointLight);

		//Light* mSpotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		//mSpotLight->SetLightColor(float3(0, 1, 0));
		//mSpotLight->SetRange(250.0f);
		//mSpotLight->SetPosition(float3(-442, 80, -16));
		//mSpotLight->SetDirection(float3(-1, 0, 0));
		//mSpotLight->SetAttenuation(1.0f, 0.0f);
		//mSpotLight->SetSpotAngle(Mathf::ToRadian(30), Mathf::ToRadian(40));
		//sceneMan.GetRootSceneNode()->AttachObject(mSpotLight);

		//{
		//	float3 direction = Normalize(float3(-111.5f, 398.1f, 3.6f) - float3(-111.1, 380.1, 73.4));
		//	for (int i = 0; i < 4; ++i)
		//	{
		//		Light* spotLight = sceneMan.CreateLight("Spot" + std::to_string(i), LT_SpotLight);
		//		spotLight->SetLightColor(float3(1, 1, 0));
		//		spotLight->SetRange(150);
		//		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, 3.6f));
		//		spotLight->SetDirection(direction);
		//		spotLight->SetAttenuation(1.0f, 0.0f);
		//		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
		//		sceneMan.GetRootSceneNode()->AttachObject(spotLight);
		//	}

		//	direction = Normalize(float3(-111.5f, 398.1f, 35.7f) - float3(-111.1, 380.1, -111.3));
		//	for (int i = 0; i < 4; ++i)
		//	{
		//		Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
		//		spotLight->SetLightColor(float3(0, 1, 1));
		//		spotLight->SetRange(150);
		//		spotLight->SetPosition(float3(-278.2f + i * 166.5f, 398.1f, -35.7f));
		//		spotLight->SetDirection(direction);
		//		spotLight->SetAttenuation(1.0f, 0.0f);
		//		spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(40));
		//		sceneMan.GetRootSceneNode()->AttachObject(spotLight);
		//	}
		//}
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

		/*if ( InputSystem::GetSingleton().KeyPress(KC_Space) )
		{
			mRenderPath->mVisualLightsWireframe = !mRenderPath->mVisualLightsWireframe;
		}*/

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

	void ValidateDepth()
	{
		int w, h;
		float* pData = nullptr;
		int r = ReadPfm("E:/Depth1.pfm", w, h, pData);

		int numGroupX = 32;
		int numGroupY = 24;

		vector<float> oData(w*h*3);

		for (int iGroupY = 0; iGroupY < numGroupY; ++iGroupY)
		{
			for (int iGroupX = 0; iGroupX < numGroupX; ++iGroupX)
			{
				float maxZ = 0;
				float minZ = Mathf::MAX_REAL;

				for (int iY = iGroupY * 32; iY < (iGroupY+1) * 32; ++iY)
				{
					for (int iX = iGroupX * 32; iX < (iGroupX+1) * 32; ++iX)
					{
						float z = pData[iY * w * 3 + iX * 3];
						minZ = std::min(minZ, z);
						maxZ = std::max(maxZ, z);

						oData[iY * w * 3 + iX * 3 + 0] = minZ;
						oData[iY * w * 3 + iX * 3 + 1] = maxZ;
						oData[iY * w * 3 + iX * 3 + 2] = 0;
					}
				}
			}
		}

		WritePfm("E:/MinMaxCPU.pfm", w, h, 3, &oData[0]);
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