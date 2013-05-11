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
#include "Graphics/Font.h"
#include "Resource/ResourceManager.h"
#include "Graphics/SpriteBatch.h"
#include "Math/Rectangle.h"
#include "Core/XMLDom.h"
#include "Core/Utility.h"

TestApp::TestApp( const String& config )
	:Application(config)
{
}


TestApp::~TestApp(void)
{
	delete mCameraControler;
}

void TestApp::Initialize()
{
	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	
	Vector3f up(0, 1, 0);
	camera->SetViewParams(Vector3f(0, 50, 150), Vector3f(0, 50, 0), up);
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 10000.0f );

	mCameraControler = new FPSCameraControler;
	//mCameraControler = new ModelViewerCameraControler();
	//mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);
}

//void TestApp::LoadContent()
//{
//	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderFactoryPtr();
//	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();
//	ResourceManager& resMan = ResourceManager::GetSingleton();
//
//	resMan.AddResource(RT_Material, "Sprite.material.xml", "General");
//	resMan.AddResource(RT_Mesh, "him.mesh", "Custom");
//
//	resMan.LoadAllFromDisk();
//
//	spriteBatch  = new SpriteBatch();
//	
//	Entity* dudeEntity = sceneManager->CreateEntity("Dude", "him.mesh",  "Custom");
//	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
//	dudeNode->SetPosition(Vector3f(0, 0, 0));
//	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
//	dudeNode->AttachObject(dudeEntity);
//
//
//	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
//	AnimationState* takeClip = animPlayer->GetClip("Take 001");
//	takeClip->WrapMode = AnimationState::Wrap_Loop;
//
//	animPlayer->PlayClip("Take 001");
//
//	String skyTexPath = FileSystem::GetSingleton().Locate("MeadowTrail.dds");
//
//	//String skyTexPath = FileSystem::GetSingleton().Locate("front.dds");
//	//mTexture = factory->CreateTextureFromFile(skyTexPath);
//	//factory->SaveTexture2D("Test.dds", texture, 0, 0);
//
//	// Sky 
//	//sceneManager->CreateSkyBox(texture, false);
//	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile(skyTexPath, 0));
//	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile("../Media/Textures/grassenvmap1024.dds", 0), 5000.0f); 
//}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	resMan.AddResource(RT_Material, "Sprite.material.xml", "General");
	resMan.AddResource(RT_Mesh, "him.mesh", "Custom");
	resMan.AddResource(RT_Font, "Consolas Regular", "General");
	resMan.LoadAllFromDisk();

	// Sprite 
	mSpriteBatch = std::make_shared<SpriteBatch>();

	// Font
	mFont = std::static_pointer_cast<Font>(resMan.GetResourceByName(RT_Font,"Consolas Regular", "General"));

	// Entity
	Entity* dudeEntity = sceneManager->CreateEntity("Dude", "him.mesh",  "Custom");
	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(Vector3f(0, 0, 0));
	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
	dudeNode->AttachObject(dudeEntity);


	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->WrapMode = AnimationState::Wrap_Loop;

	animPlayer->PlayClip("Take 001");


	mTexture = factory->CreateTextureFromFile(FileSystem::GetSingleton().Locate("brick_texture1.dds"));

	//String skyTexPath = ;
	//mTexture = factory->CreateTextureFromFile(skyTexPath);
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

	mSpriteBatch->Begin();
	mSpriteBatch->Draw(mTexture, Rectanglef(0, 0, 100, 100), ColorRGBA::Green);
	mSpriteBatch->End();
	mSpriteBatch->Flush();


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
		mMainWindow->SetTitle(ToString(fps));
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