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
	camera->SetViewParams(Vector3f(0, 10, 30), Vector3f(0, 10, 0), Vector3f(0, 1, 0));
	camera->SetProjectionParams(PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 100.0f );
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetRenderFactory();
	
	/*mBox = std::make_shared<SimpleBox>("SimpleBox");
	shared_ptr<Material> matrial = mBox->GetMaterial();
	matrial->SetDiffuseColor(ColorRGBA(1, 0, 0, 0));*/

	// Load texture
	mTexture = factory->CreateTextureFromFile("../Media/Textures/Body.dds", 0);

	
	mQuad = std::make_shared<SimpleTexturedQuad>("SimpleQuad");
	shared_ptr<Material> matrial = mQuad->GetMaterial();
	EffectParameter* para = matrial->GetEffect()->GetParameterByName("DiffuseMap");
	para->SetValue(mTexture);

	// Load mesh
	RcEngine::FileStream modelSource("../Media/Mesh/Test.mdl", RcEngine::FILE_READ);
	mDwarf = Mesh::Load(modelSource);

	// Load material 
	mDwarfMaterial = factory->CreateMaterialFromFile("SimpleTextured", "../Media/Materials/SimpleTextured.xml");
	mDwarfMaterial->SetDiffuseColor(ColorRGBA(1, 0, 0, 0));
	mDwarf->SetMaterial(mDwarfMaterial);
}

void TestApp::UnloadContent()
{
	
}



void TestApp::Render()
{
	mRenderDevice->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(1.1f, 1.1f, 1.1f, 1.0f), 1.0f, 0);

	//mQuad->Render();

	for (uint32_t i = 0; i < mDwarf->GetMeshPartCount(); ++i)
	{
	mDwarf->GetMeshPart(i)->Render();
	}

	mRenderDevice->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{
	mQuad->SetWorldMatrix( CreateScaling(10, 10, 10) * CreateTranslation(0.0f, 10.0f, -10.0f) );

	static float degree = 0;
	degree += deltaTime * 0.01f ;
	mDwarf->SetWorldMatrix( CreateRotationY(ToRadian(degree)) * CreateScaling(10, 10, 10) );

	/*static float pos = 30;
	if(mKeyboard->isKeyDown(OIS::KC_W))
	{	
		pos += 5 * deltaTime;
	}

	if(mKeyboard->isKeyDown(OIS::KC_S))
	{
		pos -= 5 * deltaTime;
	}*/
}

int32_t main()
{
	TestApp* app = new TestApp();
	app->RunGame();

	return 0;
}