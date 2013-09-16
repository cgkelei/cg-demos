#include "SponzaApp.h"
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/SamplerState.h>
#include <GUI/Label.h>
#include <GUI/UIManager.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>

SponzaApp::SponzaApp( const String& config )
	:Application(config),
	 mCameraControler(0),
	 mFramePerSecond(0)
{

}


SponzaApp::~SponzaApp(void)
{
	SAFE_DELETE(mCameraControler);
}

void SponzaApp::Initialize()
{
	InitGUI();

	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();

	//camera->SetViewParams(float3(0, 0, -20), float3(0, 0, 0));
	camera->SetViewParams(float3(-296, 147, 11), float3(0, 50, 0));
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

	mCameraControler = new FPSCameraControler;
	//mCameraControler = new ModelViewerCameraControler();
	//mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);
}

void SponzaApp::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	//FileSystem::GetSingleton().RegisterPath("../Media/Mesh/", "Custom");
	
	//Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Teapot001.mesh",  "General");
	//SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	//sponzaNode->SetPosition(float3(0, 0, 0));
	//sponzaNode->SetScale(0.05f);
	//sponzaNode->AttachObject(sponzaEntity);

	//Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	//SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	//sponzaNode->SetPosition(float3(0, 0, 0));
	//sponzaNode->SetScale(0.45f);
	//sponzaNode->AttachObject(sponzaEntity);

	/*Entity* dudeEntity = sceneMan.CreateEntity("Dude", "him.mesh",  "Custom");
	SceneNode* dudeNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(float3(0, 0, 0));
	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(90.0f), 0.0f, 0.0f));
	dudeNode->AttachObject(dudeEntity);

	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->WrapMode = AnimationState::Wrap_Loop;
	animPlayer->PlayClip("Take 001");*/

	Entity* arthasEntity = sceneMan.CreateEntity("Arthas", "Arthas/Mesh_ArthasLichKing.mesh",  "Custom");
	SceneNode* arthasNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Arthas");
	arthasNode->SetPosition(float3(0, 0, 0));
	//dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(90.0f), 0.0f, 0.0f));
	arthasNode->AttachObject(arthasEntity);

	AnimationPlayer* arthasAnimPlayer = arthasEntity->GetAnimationPlayer();
	AnimationState* arthasTakeClip = arthasAnimPlayer->GetClip("Take 001");
	arthasTakeClip->WrapMode = AnimationState::Wrap_Loop;	
}

void SponzaApp::UnloadContent()
{

}

void SponzaApp::CalculateFrameRate()
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

void SponzaApp::Render()
{
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();
	SceneManager& scenenMan = Context::GetSingleton().GetSceneManager();

	shared_ptr<FrameBuffer> currentFrameBuffer = renderDevice.GetCurrentFrameBuffer();

	renderDevice.BindFrameBuffer(currentFrameBuffer);

	float clr = (float)169/255;
	currentFrameBuffer->Clear(CF_Color | CF_Depth |CF_Stencil, ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);

	//DrawUI();

	// Move to engine level
	/*scenenMan.UpdateRenderQueue(currentFrameBuffer->GetCamera(), RO_StateChange);
	scenenMan.RenderScene();*/

	RenderQueue* renderQueue = scenenMan.GetRenderQueue();
	std::vector<RenderQueueItem>&  renderBucket = renderQueue->GetRenderBucket(RenderQueue::BucketOpaque);

	if (renderBucket.size())
	{
		std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
			return lhs.SortKey < rhs.SortKey; });

			for (const RenderQueueItem& renderItem : renderBucket)
			{

				renderItem.Renderable->Render();
			}
	}

	// Swap Buffer
	currentFrameBuffer->SwapBuffers();
}

void SponzaApp::Update( float deltaTime )
{
	CalculateFrameRate();
	mMainWindow->SetTitle("Graphics Demo FPS:" + std::to_string(mFramePerSecond));
}

void SponzaApp::InitGUI()
{
	mFPSLabel = new Label();
	mFPSLabel->InitGuiStyle(nullptr);
	mFPSLabel->SetName("FPSLabel");
	mFPSLabel->SetPosition(int2(400, 500));
	mFPSLabel->SetSize(int2(400, 100));
	UIManager::GetSingleton().GetRoot()->AddChild( mFPSLabel );	
}

void SponzaApp::DrawUI()
{
	wchar_t buffer[100];
	int cx = swprintf ( buffer, 100, L"Graphics Demo FPS: %d", mFramePerSecond );
	mFPSLabel->SetText(buffer);

	// Render UI
	UIManager::GetSingleton().Render();
}



int32_t main()
{
	SponzaApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}