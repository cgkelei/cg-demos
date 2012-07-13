#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/Effect.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/SimpleGeometry.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"

using namespace RcEngine::Math;

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
	camera->SetProjectionParams(MathUtil::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 100.0f );
}


void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetRenderFactory();
	mBox = std::make_shared<SimpleBox>("SimpleBox");	
}

void TestApp::UnloadContent()
{
	
}

using namespace RcEngine::Render;

void TestApp::Render()
{
	mRenderDevice->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(1, 1, 1, 1), 1.0f, 0);

	mBox->Render();

	mRenderDevice->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{
	mBox->SetWorldMatrix(MathUtil::CreateScaling(5, 5, 5) * MathUtil::CreateTranslation(0.0f, 0.0f, 5.0f));

	if(mKeyboard->isKeyDown(OIS::KC_W))
	{
		Camera* camera = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
		static float pos = 30;
		camera->SetViewParams(Vector3f(0, 0, pos), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
		pos += 1;
	}
}

int32_t main()
{
	TestApp* app = new TestApp();
	app->RunGame();
	return 0;
}