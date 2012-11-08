#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/Effect.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Camera.h"
#include "Graphics/CameraControler.h"
#include "Graphics/SimpleGeometry.h"
#include "Graphics/Animation.h"
#include "Graphics/AnimationClip.h"
#include "Graphics/AnimationState.h"
#include "Graphics/Font.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"
#include "IO/FileStream.h"
#include "IO/FileSystem.h"
#include "MainApp/Window.h"
#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include "Scene/SceneNode.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Graphics/Sky.h"
#include "Graphics/SimpleGeometry.h"
#include "Resource/ResourceManager.h"
#include "Graphics/SpriteBatch.h"
#include <D3DX10Math.h>
#include "Math/Rectangle.h"
#include "Core/XMLDom.h"

#pragma comment(lib, "D3DX10.lib")


SpriteBatch* spriteBatch;

TestApp::TestApp( const String& config )
	:Application(config)
{

}


TestApp::~TestApp(void)
{
}

void TestApp::Initialize()
{
	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	
	Vector3f up(0, 1, 0);
	camera->SetViewParams(Vector3f(0, 100, 400), Vector3f(0, 100, 0), up);
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 10000.0f );

	mCameraControler = new FPSCameraControler;
	//mCameraControler = new ModelViewerCameraControler();
	//mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	/*shared_ptr<Font> font =  
		std::static_pointer_cast<Font>(
		resMan.GetResourceByHandle(resMan.AddResource(ResourceTypes::Font, "VeraMoBI.ttf", "General"))) ;

	font->Load();

	auto face = font->GetFace(10);
	
	auto h = resMan.AddResource(ResourceTypes::Material, "SkinModel.material.xml", "Custom");
	resMan.GetResourceByHandle(h)->Load();*/

	resMan.AddResource(ResourceTypes::Material, "Sprite.material.xml", "General");
	resMan.AddResource(ResourceTypes::Mesh, "him.mesh", "Custom");
	//resMan.AddResource(ResourceTypes::Mesh, "Teapot.mesh", "Custom");
	//resMan.AddResource(ResourceTypes::Pipeline, "DeferredLighting.pipeline.xml", "General");

	resMan.LoadAllFromDisk();

	spriteBatch  = new SpriteBatch();
	
	Entity* dudeEntity = sceneManager->CreateEntity("Dude", "him.mesh",  "Custom");
	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(Vector3f(0, 0, 0));
	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
	dudeNode->AttachObject(dudeEntity);


	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->WrapMode = AnimationState::Wrap_Loop;

	animPlayer->PlayClip("Take 001");


	/*Entity* dudeEntity1 = sceneManager->CreateEntity("Dude1", "dude.mdl",  "Custom");
	SceneNode* dudeNode1 = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dude1");
	dudeNode1->SetPosition(Vector3f(100, 0, 0));
	dudeNode1->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
	dudeNode1->AttachObject(dudeEntity1);


	AnimationPlayer* animPlayer1 = dudeEntity1->GetAnimationPlayer();
	AnimationState* takeClip1 = animPlayer1->GetClip("Take 001");
	takeClip1->WrapMode = AnimationState::Wrap_Loop;

	animPlayer1->PlayClip("Take 001");*/

	//Entity* tinyEntity = sceneManager->CreateEntity("dance", "dance.mdl");
	//SceneNode* tinyNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("dance");
	////tinyNode->SetScale(Vector3f(0.3, 0.3, 0.3));
	//tinyNode->SetRotation(QuaternionFromRotationYawPitchRoll(0.0f, Mathf::ToRadian(-90.0f), 0.0f));
	//tinyNode->AttachObject(tinyEntity);


	//shared_ptr<Animation> animPlayer = tinyEntity->GetAnimationPlayer();
	//AnimationClip* takeClip = animPlayer->GetClip("Take 001");
	//takeClip->GetAnimationState()->SetAnimationWrapMode(AnimationState::Wrap_Loop);
	//takeClip->Play();


	/*vector<String> skyTextures;
	skyTextures.push_back( String("../Media/Textures/left.dds"));
	skyTextures.push_back( String("../Media/Textures/right.dds"));
	skyTextures.push_back( String("../Media/Textures/up.dds"));
	skyTextures.push_back( String("../Media/Textures/down.dds"));
	skyTextures.push_back( String("../Media/Textures/front.dds"));
	skyTextures.push_back( String("../Media/Textures/back.dds"));

	auto texture = factory->CreateTextureArrayFromFile(skyTextures);*/

	//String skyTexPath = FileSystem::GetSingleton().Locate("MeadowTrail.dds");

	String skyTexPath = FileSystem::GetSingleton().Locate("front.dds");
	mTexture = factory->CreateTextureFromFile(skyTexPath);
	//factory->SaveTexture2D("Test.dds", texture, 0, 0);

	// Sky 
	//sceneManager->CreateSkyBox(texture, false);
	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile(skyTexPath, 0));
	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile("../Media/Textures/grassenvmap1024.dds", 0), 5000.0f); 
}


void TestApp::UnloadContent()
{

}



void TestApp::Render()
{
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();
	SceneManager& scenenMan = Context::GetSingleton().GetSceneManager();

	spriteBatch->Begin();
	spriteBatch->Draw(mTexture, Vector2f(0, 0), ColorRGBA::White);
	spriteBatch->End();
	spriteBatch->Flush();


	shared_ptr<FrameBuffer> currentFrameBuffer = renderDevice.GetCurrentFrameBuffer();

	float clr = (float)169/255;
	currentFrameBuffer->Clear(CF_Color | CF_Depth |CF_Stencil, RcEngine::ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);

	scenenMan.UpdateRenderQueue(currentFrameBuffer->GetCamera(), RO_StateChange);

	Context::GetSingleton().GetSceneManager().RenderScene();

	currentFrameBuffer->SwapBuffers();

	CalculateFrameRate();
}


void TestApp::Update( float deltaTime )
{
	/*static float degree = 0;
	degree += deltaTime * 1.0f;*/

	//SceneNode* dwarfNode = Context::GetSingleton().GetSceneManager().FindSceneNode("Dwarf");
	//dwarfNode->SetRotation(QuaternionFromRotationMatrix(mCameraControler->GetWorldMatrix()));
}

void TestApp::CalculateFrameRate()
{
	static int frameCount = 0;
	static float baseTime = 0;

	frameCount++;

	if (mTimer.GetGameTime()-baseTime >= 1.0f)
	{
		float fps = (float)frameCount;
		std::stringstream sss; 
		sss << "FPS: " << fps;
		mMainWindow->SetTitle(sss.str());
		frameCount = 0;
		baseTime += 1.0f;
	}
}


int32_t main()
{
	{
		TestApp app("Config.xml");
		app.Create();
		app.RunGame();
		app.Release();
	}
	
	return 0;
}