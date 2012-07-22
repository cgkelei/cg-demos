#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/Effect.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/SimpleGeometry.h"
#include "Content/MeshContentLoader.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"
#include "IO/FileStream.h"
#include <D3DX10Math.h>

#pragma comment(lib, "D3DX10.lib")

using namespace RcEngine::Math;
using namespace RcEngine::Render;
using namespace RcEngine::Content;

TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

void TestApp::Initialize()
{
	Application::Initialize();

	Camera* camera = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	camera->SetViewParams(Vector3f(0, 0, 30), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
	camera->SetProjectionParams(PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 100.0f );
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetRenderFactory();
	
	mBox = std::make_shared<SimpleBox>("SimpleBox");
	shared_ptr<Material> matrial = mBox->GetMaterial();
	matrial->SetDiffuseColor(ColorRGBA(1, 0, 0, 0));

	//Content::MeshContent* test = Content::ReadMeshContent("../Media/Mesh/test.xml");

	RcEngine::FileStream modelSource("../Media/Mesh/Test.mdl", RcEngine::FILE_READ);

	shared_ptr<Mesh> dwarf = Mesh::Load(modelSource);
}

void TestApp::UnloadContent()
{
	
}



void TestApp::Render()
{
	mRenderDevice->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(1, 1, 1, 1), 1.0f, 0);

	mBox->Render();

	mRenderDevice->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{
	mBox->SetWorldMatrix(CreateScaling(5, 5, 5) * CreateTranslation(0.0f, 0.0f, 5.0f));

	static float pos = 30;
	if(mKeyboard->isKeyDown(OIS::KC_W))
	{	
		pos += 5 * deltaTime;
	}

	if(mKeyboard->isKeyDown(OIS::KC_S))
	{
		pos -= 5 * deltaTime;
	}

	Camera* camera = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	camera->SetViewParams(Vector3f(0, 0, pos), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
}

int32_t main()
{
	TestApp* app = new TestApp();
	app->RunGame();

	return 0;
}