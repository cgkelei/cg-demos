#include <MainApp/Application.h>
#include <Graphics/CameraController1.h>
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
#include <Graphics/Pipeline.h>
#include <Graphics/Renderer.h>
#include <Graphics/FrameBuffer.h>
#include <GUI/Label.h>
#include <GUI/UIManager.h>
#include <IO/FileSystem.h>
#include <Math/MathUtil.h>
#include <Graphics/CascadedShadowMap.h>
#include <Scene/Light.h>

using namespace RcEngine;

class SponzaApp : public Application
{
public:
	SponzaApp(const String& config)
		:Application(config), mCameraControler(0), mFramePerSecond(0)
	{

	}
	~SponzaApp(void)
	{

	}
protected:

	void Initialize() 
	{
		mSceneRender = new Renderer;
		//mSceneRender->Init();

		mCascadedShadowMap = new CascadedShadowMap;
	}

	void LoadContent()
	{
		RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
		SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
		ResourceManager& resMan = ResourceManager::GetSingleton();
		RenderDevice& device = Context::GetSingleton().GetRenderDevice();

		/*std::cout << "load DeferredLighting.pipeline.xml\n";
		mPipeline = std::static_pointer_cast<Pipeline>( 
			resMan.GetResourceByName(RT_Pipeline, "Fbx.pipeline.xml", "General"));*/

		mCamera = device.GetScreenFrameBuffer()->GetCamera();
		/*mCamera->SetViewParams(float3(85, 68, -21), float3(86, 68, -21));*/
		/*mCamera->SetViewParams(float3(-211, 57, -12), float3(-212, 57, -12));*/
		mCamera->CreateLookAt(float3(-104, 39, -9.8), float3(-54,34.9,-10.8));
		mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 3000.0f );

		mCameraControler = new RcEngine::Test::FPSCameraControler;
		mCameraControler->AttachCamera(*mCamera);

		// Load Scene
		Entity* sponzaEntity = sceneMan.CreateEntity("Sponza", "Sponza.mesh",  "Custom");
		SceneNode* sponzaNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Sponza");
		sponzaNode->SetPosition(float3(0, 0, 0));
		sponzaNode->SetScale(0.45f);
		sponzaNode->AttachObject(sponzaEntity);

		mCameraControler->SetMoveSpeed(50.0f);
		mCameraControler->SetMoveInertia(true);

		//mSceneRender->SetRenderPipeline(mPipeline);

		mDirLight = sceneMan.CreateLight("Sun");
		mDirLight->SetLightType(LT_Directional);
		mDirLight->SetDirection(float3(0, -1, -1));
		mDirLight->SetLightColor(float3(1, 1, 1));
		mDirLight->SetCastShadow(true);
		sceneMan.GetRootSceneNode()->AttachObject(mDirLight);
	}

	void UnloadContent() {}

	void Update(float deltaTime) 
	{
		mCameraControler->Update(deltaTime);
	}

	void Render()
	{
		//mSceneRender->RenderScene();
		SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
		RenderDevice& device = Context::GetSingleton().GetRenderDevice();

		mCascadedShadowMap->UpdateShadowMatrix(*mCamera, *mDirLight);

		// Update shadow map matrix
		shared_ptr<FrameBuffer>& shadowFrameBuffer = mCascadedShadowMap->mShadowFrameBuffer;

		// Draw all shadow map
		for (uint32_t i = 0; i < mDirLight->GetShadowCascades(); ++i)
		{		
			shadowFrameBuffer->SetViewport(mCascadedShadowMap->mShadowVP[i]);
			shadowFrameBuffer->SetCamera(mCascadedShadowMap->mLightCamera[i]);

			//float4x4 viewMatrix = shadowFrameBuffer->GetCamera()->GetViewMatrix();
			//float4x4 projMatrix = shadowFrameBuffer->GetCamera()->GetEngineProjMatrix();

			//float4x4 viewProjMatrix = viewMatrix * projMatrix;
			//float3 coord = TransformCoord(float3(0, 0, 0), viewProjMatrix);

			device.BindFrameBuffer(shadowFrameBuffer);	

			bool b = shadowFrameBuffer->CheckFramebufferStatus();
				
			const Camera& camera = *(device.GetCurrentFrameBuffer()->GetCamera());	
			sceneMan.UpdateRenderQueue(camera, RO_None);

			RenderQueue* renderQueue = sceneMan.GetRenderQueue();	
			for (const auto& kv : renderQueue->GetAllRenderBuckets())
			{
				RenderBucket& renderBucket = *(kv.second);

				if (renderBucket.size())
				{
					std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
						return lhs.SortKey < rhs.SortKey; });

						for (const RenderQueueItem& renderItem : renderBucket) 
						{
							renderItem.Renderable->GetMaterial()->SetCurrentTechnique("SSM");
							renderItem.Renderable->Render();
						}
				}
			}
		}

		device.GetRenderFactory()->SaveTexture2D("E:/Depth.pfm", mCascadedShadowMap->mShadowTexture, 0, 0);

		device.BindFrameBuffer(device.GetScreenFrameBuffer());
		device.GetScreenFrameBuffer()->SwapBuffers();
	}

	void CalculateFrameRate()
	{
	}

protected:
	RcEngine::Test::FPSCameraControler* mCameraControler;
	shared_ptr<Camera> mCamera;
	int mFramePerSecond;
	shared_ptr<Pipeline> mPipeline;
	Renderer* mSceneRender;

	Light* mDirLight;
	CascadedShadowMap* mCascadedShadowMap;
};


int main(int argc, char* argv[])
{
	SponzaApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}