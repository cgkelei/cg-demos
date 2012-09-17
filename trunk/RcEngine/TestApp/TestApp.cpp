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
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"
#include "IO/FileStream.h"
#include "MainApp/Window.h"
#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include "Scene/SceneNode.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Graphics/Sky.h"
#include "Graphics/SimpleGeometry.h"
#include <D3DX10Math.h>

#pragma comment(lib, "D3DX10.lib")


TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

void TestApp::Initialize()
{
	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	
	Vector3f up(0, 1, 0);
	camera->SetViewParams(Vector3f(0, 0, 40), Vector3f(0, 0, 0), up);
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
	
	mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Dwarf/Body.material.xml");
	Entity* dwarfEntity = sceneManager->CreateEntity("Dwarf", "../Media/Mesh/Dwarf/Dwarf.mdl");
	dwarfEntity->SetMaterial(mDwarfMaterial);

	SceneNode* dwarfNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dwarfNode->SetPosition(Vector3f(0, 0, 0));


	BoundingSpheref spehre = dwarfEntity->GetBoundingSphere();
	SceneNode* centerNode = dwarfNode->CreateChildSceneNode("Center", -spehre.Center);
	centerNode->AttachObject(dwarfEntity);

	dwarfNode->SetScale( Vector3f(10.0f / spehre.Radius, 10.0f / spehre.Radius, 10.0f / spehre.Radius) );


	// Sky 
	/*sceneManager->CreateSkyBox(
		factory->CreateTextureFromFile("../Media/Textures/front.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/back.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/left.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/right.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/up.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/down.dds", 0), 5000.0f);*/

	sceneManager->CreateSkyBox(factory->CreateTextureFromFile("../Media/Textures/grassenvmap1024.dds", 0), 5000.0f); 
}


void TestApp::UnloadContent()
{

}



void TestApp::Render()
{
	//RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();
	//device->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
	//	RcEngine::ColorRGBA(1.1f, 1.1f, 1.1f, 1.0f), 1.0f, 0);
	//device->GetCurrentFrameBuffer()->SwapBuffers();
}


void TestApp::Update( float deltaTime )
{
	/*static float degree = 0;
	degree += deltaTime * 1.0f;*/

	//SceneNode* dwarfNode = Context::GetSingleton().GetSceneManager().FindSceneNode("Dwarf");

	//dwarfNode->SetRotation(QuaternionFromRotationMatrix(mCameraControler->GetWorldMatrix()));
}


int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}