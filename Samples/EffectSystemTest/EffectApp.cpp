#include "EffectApp.h"
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Resource/ResourceManager.h>
#include <Graphics/Camera.h>
#include <Graphics/AnimationController.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Scene/Light.h>
#include <Core/Context.h>
#include <Math/MathUtil.h>
#include <Graphics/Texture.h>


EffectApp::EffectApp(const String& config)
	:Application(config)
{
}


EffectApp::~EffectApp(void)
{
}

void EffectApp::Initialize()
{
	mDeferredPath = new DeferredPath();
	mDeferredPath->OnGraphicsInit();
}

void EffectApp::InitGUI()
{
	
}

void EffectApp::LoadContent()
{
	ResourceManager& resMan = ResourceManager::GetSingleton();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	// Camera
	mCamera = device.GetScreenFrameBuffer()->GetCamera();

	mCamera->CreateLookAt(float3(-137.0, 97.3, 82.0), float3(-136.5, 96.8, 81.3), float3(0.3, 0.9, -0.4));
	mCamera->CreatePerspectiveFov(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );

	mCameraControler = new RcEngine::Test::FPSCameraControler;
	mCameraControler->AttachCamera(*mCamera);
	mCameraControler->SetMoveSpeed(100.0f);
	mCameraControler->SetMoveInertia(true);


	// Ground
	Entity* entity = sceneMan.CreateEntity("Ground", "./Geo/Ground.mesh",  "Custom");
	SceneNode* sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Ground");
	sceneNode->SetScale(float3(2.5,2.5,2.5));
	sceneNode->SetPosition(float3(0, 0, 0));
	sceneNode->AttachObject(entity);

	// Teapot 
	//entity = sceneMan.CreateEntity("Teapot", "./Geo/Teapot.mesh",  "Custom");
	//sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Teapot");
	//sceneNode->Rotate(QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::ToRadian(-90.0)));
	//sceneNode->SetPosition(float3(50, 0, 0));
	//sceneNode->AttachObject(entity);

	entity = sceneMan.CreateEntity("Nanosuit", "./Nanosuit/Nanosuit.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Nanosuit");
	sceneNode->SetScale(float3(2,2,2));
	sceneNode->SetPosition(float3(-50,0,0));
	sceneNode->AttachObject(entity);
	auto b = entity->GetWorldBoundingBox();

	entity = sceneMan.CreateEntity("Tank", "./Tank/tank.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Tank");
	sceneNode->SetScale(float3(10, 10, 10));
	sceneNode->SetPosition(float3(0,0,0));
	sceneNode->AttachObject(entity);

	// Skin dude
	entity = sceneMan.CreateEntity("Dude", "./Dude/dude.mesh",  "Custom");
	sceneNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Dude");
	sceneNode->SetScale(float3(0.5, 0.5, 0.5));
	sceneNode->SetPosition(float3(50,0,0));
	sceneNode->SetWorldRotation(QuaternionFromRotationAxis(float3(0, 1, 0), Mathf::PI));
	sceneNode->AttachObject(entity);

	AnimationPlayer* animPlayer = entity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->SetAnimationWrapMode(AnimationState::Wrap_Loop);
	animPlayer->PlayClip("Take 001");

	// Load Skybox
	shared_ptr<TextureResource> skyBox = resMan.GetResourceByName<TextureResource>(RT_Texture, "MeadowTrail.dds", "General");
	sceneMan.CreateSkyBox(skyBox->GetTexture());

	Light* spotLight = sceneMan.CreateLight("Spot", LT_SpotLight);
	spotLight->SetDirection(float3(0, -1.5, -1));
	spotLight->SetLightColor(float3(1, 1, 1));
	spotLight->SetRange(300.0);
	spotLight->SetPosition(float3(0.0f, 150.0f, 100.0f));
	spotLight->SetAttenuation(1.0f, 0.0f);
	spotLight->SetSpotAngle(Mathf::ToRadian(10), Mathf::ToRadian(60));
	spotLight->SetCastShadow(true);
	spotLight->SetSpotlightNearClip(10);
	sceneMan.GetRootSceneNode()->AttachObject(spotLight);
}

void EffectApp::Update( float deltaTime )
{
	mCameraControler->Update(deltaTime);
}

void EffectApp::Render()
{
	mDeferredPath->RenderScene();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	device.GetScreenFrameBuffer()->SwapBuffers();
}

void EffectApp::CalculateFrameRate()
{

}


int main(int argc, char* argv[])
{
	EffectApp app("Config.xml");
	app.Create();
	app.RunGame();
	app.Release();

	return 0;
}