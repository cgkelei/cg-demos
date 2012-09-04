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
#include "Content/MeshContentLoader.h"
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
	//camera->SetViewParams(eye, target, up);
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );

	//mCameraControler = new FPSCameraControler;
	mCameraControler = new ModelViewerCameraControler();
	mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();

	SceneNode* dwarfNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	SceneNode* modelCenterNode = dwarfNode->CreateChildSceneNode("center");
		
	Entity* dwarfEntity = sceneManager->CreateEntity("Dwarf", "../Media/Mesh/Dwarf/Dwarf.mdl");
	mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Dwarf/Body.material.xml");
	dwarfEntity->SetMaterial(mDwarfMaterial);

	const Vector3f& center = dwarfEntity->GetBoundingSphere().Center;
	float radius = dwarfEntity->GetBoundingSphere().Radius;

	modelCenterNode->AttachObject(dwarfEntity);
	modelCenterNode->Translate(-center, Node::TS_Local);
	dwarfNode->SetScale(Vector3f(10.0f / radius, 10.0f / radius, 10.0f / radius));

	// Sky 

	/*SkyBox* skyBox = sceneManager->CreateSkyBox(
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0),
		factory->CreateTextureFromFile("../Media/Textures/Glass.dds", 0));

	sceneManager->GetRootSceneNode()->AttachObject(skyBox);*/
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
	static float degree = 0;
	degree += deltaTime * 1.0f;

	SceneNode* dwarfNode = Context::GetSingleton().GetSceneManager().FindSceneNode("Dwarf");
	dwarfNode->SetRotation(QuaternionFromRotationMatrix(mCameraControler->GetWorldMatrix()));
}

int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}