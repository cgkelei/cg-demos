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
	camera->SetProjectionParams(PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );

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
	RcEngine::FileStream modelSource("../Media/Mesh/Dwarf/Dwarf.mdl", RcEngine::FILE_READ);
	mDwarf = Mesh::Load(modelSource);

	mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Dwarf/Body.material.xml");
	//
	//mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Test/Armor.material.xml");
	mDwarf->SetMaterial(mDwarfMaterial);

	new SceneManager;

	SceneManager& sceneManager = Context::GetSingleton().GetSceneManager();

	SceneNode* dwarfNode = sceneManager.GetRootSceneNode()->CreateChildSceneNode("dwarf");
	SceneNode* dwarfChildOneNode = dwarfNode->CreateChildSceneNode("dwarf one");
	SceneNode* dwarfChildTwoNode = dwarfNode->CreateChildSceneNode("dwarf two");
}


void TestApp::UnloadContent()
{
	
}



void TestApp::Render()
{
	RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();

	device->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::ColorRGBA(1.1f, 1.1f, 1.1f, 1.0f), 1.0f, 0);


	for (uint32_t i = 0; i < mDwarf->GetMeshPartCount(); ++i)
	{
		mDwarf->GetMeshPart(i)->Render();
	}

	device->GetCurrentFrameBuffer()->SwapBuffers();
}


void TestApp::Update( float deltaTime )
{
	static float degree = 0;
	//degree += deltaTime * 90 ;

	Quaternionf quat = QuaternionFromRotationYawPitchRoll( ToRadian(degree), 0.0f, 0.0f);

	const Vector3f& center = mDwarf->GetBoundingSphere().Center;
	float radius = mDwarf->GetBoundingSphere().Radius;

	Matrix4f worldScale = CreateTranslation(-center.X(), -center.Y(), -center.Z());
	//worldScale.MakeIdentity();
	worldScale = worldScale * CreateScaling(10.0f / radius, 10.0f / radius, 10.0f / radius);
	Matrix4f world = mCameraControler->GetWorldMatrix();

	mDwarf->SetWorldMatrix( worldScale * world );

}

int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}