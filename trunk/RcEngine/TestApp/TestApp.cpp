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
#include <D3DX10Math.h>

#pragma comment(lib, "D3DX10.lib")


TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

bool Equal(const D3DXMATRIX& mat1, const Matrix4f& mat2)
{
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j)
		{
			if (mat1(i, j) != mat2(i,j))
			{
				float a = mat1(i, j);
				float b = mat2(i,j);
				return false;
			}
		}
	return true;
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
	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderDevice().GetRenderFactory();

	// Load mesh
	//RcEngine::FileStream modelSource("../Media/Mesh/Test/Test.mdl", RcEngine::FILE_READ);
	//RcEngine::FileStream modelSource("../Media/Mesh/Dwarf/Dwarf.mdl", RcEngine::FILE_READ);
	//mDwarf = Mesh::Load(modelSource);

	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();

	SceneNode* dwarfNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	Entity* dwarfEntity = sceneManager->CreateEntity("Dwarf", "../Media/Mesh/Dwarf/Dwarf.mdl");
	

	mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Dwarf/Body.material.xml");
	dwarfEntity->SetMaterial(mDwarfMaterial);

	dwarfNode->AttachObject(dwarfEntity);

	//mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Test/Armor.material.xml");
	//mDwarf->SetMaterial(mDwarfMaterial);

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
	//degree += deltaTime * 90 ;

	SceneNode* dwarfNode = Context::GetSingleton().GetSceneManager().FindSceneNode("Dwarf");
	Entity* dwarfEntity = static_cast<Entity*>( dwarfNode->GetAttachedObject("Dwarf") );

	//Quaternionf quat = QuaternionFromRotationYawPitchRoll( Mathf::ToRadian(degree), 0.0f, 0.0f);

	const Vector3f& center = dwarfEntity->GetBoundingSphere().Center;
	float radius = dwarfEntity->GetBoundingSphere().Radius;

	Matrix4f worldScale = CreateTranslation(-center.X(), -center.Y(), -center.Z());

	worldScale = worldScale * CreateScaling(10.0f / radius, 10.0f / radius, 10.0f / radius);
	Matrix4f world = worldScale * mCameraControler->GetWorldMatrix();

	Quaternionf quat;
	Vector3f pos, scale;

	MatrixDecompose(scale, quat, pos, world);

	dwarfNode->SetWorldTransform(pos, quat, scale);

}

int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}