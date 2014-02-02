#include "App.h"
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/SamplerState.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <GUI/UIManager.h>
#include <GUI/Label.h>
#include <GUI/UIWindow.h>

App::App( const String& config )
	: Application(config),
	  mSceneRender(nullptr),
	  mFramePerSecond(0)
{

}

App::~App(void)
{
	delete mSceneRender;
}

void App::Initialize()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory =  Context::GetSingleton().GetRenderFactory();

	mSceneRender = new Renderer;
	mSceneRender->Init();

	InitGUI();
}

void App::LoadContent()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	std::cout << "load DeferredLighting.pipeline.xml\n";
	// Load pipeline
	mDeferredPipeline = std::static_pointer_cast<Pipeline>( 
		resMan.GetResourceByName(RT_Pipeline, "DeferredLighting.pipeline.xml", "General"));

	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->SetScale(0.45f);
	sponzaNode->AttachObject(sponzaEntity);

	// Set up lights
	SetupLights();
	
	// Set up camera
	mCamera = device.GetScreenFrameBuffer()->GetCamera();
	mCamera->SetViewParams(float3(-296, 147, 11), float3(0, 147, 0));
	mCamera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);

	mCameraControler->SetMoveSpeed(50.0f);
	mCameraControler->SetMoveInertia(true);



	mSceneRender->SetRenderPipeline(mDeferredPipeline);
}

void App::SetupLights()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();

	Light* sunLight = sceneMan.CreateLight("Sun");
	sunLight->SetLightType(LT_Directional);
	sunLight->SetDirection(float3(-1, -1, -1));
	sunLight->SetLightColor(float3(1, 1, 1));
	sceneMan.GetRootSceneNode()->AttachObject(sunLight);

	//Light* pointLight = sceneMan.CreateLight("Point");
	//pointLight->SetLightType(LT_Point);
	//pointLight->SetLightColor(float3(1, 0, 0));
	//pointLight->SetAttenuation(80.0f);
	//pointLight->SetPosition(float3(450, 78, -27));
	//sceneMan.GetRootSceneNode()->AttachObject(pointLight);
}

void App::UnloadContent()
{
}

void App::Render()
{
	mSceneRender->RenderScene();
}

void App::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);
	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));
}

void App::CalculateFrameRate()
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

	const float3& cameraPos = mCamera->GetPosition();

	wchar_t buffer[255];
	int cx = swprintf (buffer, 255, L"Graphics Demo FPS: %d, Camera Pos (%.1f, %.1f, %.1f)", mFramePerSecond,
		cameraPos.X(), cameraPos.Y(), cameraPos.Z());
	mLabel->SetText(buffer);

}

void App::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mLabel = new Label();
	mLabel->InitGuiStyle(nullptr);
	mLabel->SetName("FPSLabel");
	mLabel->SetPosition(int2(10, 700));
	mLabel->SetTextColor(ColorRGBA(1, 0, 0, 1));
	mLabel->SetSize(int2(800, 100));
	//mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mLabel );	

	mWindow = new UIWindow;
	mWindow->InitGuiStyle(nullptr);
	mWindow->SetName("Panel");
	mWindow->SetTitle(L"Window Title");
	mWindow->SetPosition(int2(20, 50));
	mWindow->SetSize(int2(350, 300));
	rootElem->AddChild( mWindow );	
}

void App::WindowResize(uint32_t width, uint32_t height)
{
	// resize pipeline buffer
	Camera& camera = *(Context::GetSingleton().GetRenderDevice().GetScreenFrameBuffer()->GetCamera());
	camera.SetProjectionParams(camera.GetFov(), (float)width/(float)height, camera.GetNearPlane(), camera.GetFarPlane());

	mDeferredPipeline->Resize(width, height);
}


int main(int argc, char* argv[])
{
	App app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();


	return 0;
}