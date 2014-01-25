#include "App.h"
#include <MainApp/Window.h>
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
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

App::App( const String& config )
	: Application(config)
{

}

App::~App(void)
{
}

void App::Initialize()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory =  Context::GetSingleton().GetRenderFactory();

	// Create GBuffer
	const uint32_t GBufferWidth = mMainWindow->GetWidth();
	const uint32_t GBufferHeight = mMainWindow->GetHeight();

	mGbuffer = factory.CreateFrameBuffer(GBufferWidth, GBufferHeight);
	
	device.BindFrameBuffer(mGbuffer);

	shared_ptr<Texture> depthRT = factory.CreateTexture2D(GBufferWidth, GBufferHeight, PF_Depth32, 1, 1, 1, 1, 0, nullptr);
	mGbuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(depthRT, 0, 0));

	shared_ptr<Texture> albedoRT = factory.CreateTexture2D(GBufferWidth, GBufferHeight, PF_R8G8B8A8, 1, 1, 1, 1, 0, nullptr);
	shared_ptr<Texture> normalRT = factory.CreateTexture2D(GBufferWidth, GBufferHeight, PF_R8G8B8A8, 1, 1, 1, 1, 0, nullptr);
	mGbuffer->Attach(ATT_Color0, factory.CreateRenderTargetView2D(albedoRT, 0, 0));
	mGbuffer->Attach(ATT_Color1, factory.CreateRenderTargetView2D(normalRT, 0, 0));

	mGbuffer->CheckFramebufferStatus();

	// Bind Default ScreenBuffer
	device.BindFrameBuffer(device.GetScreenFrameBuffer());
}

void App::LoadContent()
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	// Load Scene
	Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
	SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
	sponzaNode->SetPosition(float3(0, 0, 0));
	sponzaNode->SetScale(0.45f);
	sponzaNode->AttachObject(sponzaEntity);

	// Set up camera
	Camera* camera = device.GetCurrentFrameBuffer()->GetCamera();
	camera->SetViewParams(float3(-296, 147, 11), float3(0, 50, 0));
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );
}

void App::UnloadContent()
{
}

void App::Render()
{
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();
	SceneManager& scenenMan = Context::GetSingleton().GetSceneManager();

	shared_ptr<FrameBuffer> screenFB = renderDevice.GetScreenFrameBuffer();
	
	scenenMan.UpdateRenderQueue(screenFB->GetCamera(), RO_StateChange);

	RenderQueue* renderQueue = scenenMan.GetRenderQueue();	
	std::vector<RenderQueueItem>&  renderBucket = renderQueue->GetRenderBucket(RenderQueue::BucketOpaque);
	
	renderDevice.BindFrameBuffer(mGbuffer);	
	mGbuffer->Clear(CF_Color | CF_Depth , ColorRGBA::Black, 1.0f, 0);

	if (renderBucket.size())
	{
		std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
			return lhs.SortKey < rhs.SortKey; });

			for (const RenderQueueItem& renderItem : renderBucket)
			{

				renderItem.Renderable->Render();
			}
	}

	renderDevice.BindFrameBuffer(screenFB);	
	screenFB->Clear(CF_Color | CF_Depth | CF_Stencil, ColorRGBA::Black, 1.0f, 0);
	screenFB->SwapBuffers();	
}

void App::Update( float deltaTime )
{

}


int main(int argc, char* argv[])
{
	App app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();


	return 0;
}