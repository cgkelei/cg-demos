#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Math/ColorRGBA.h"

TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

void TestApp::LoadContent()
{

}

void TestApp::UnloadContent()
{

}

using namespace RcEngine::RenderSystem;

void TestApp::Render()
{
	mRenderDevice->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(0, 1, 1, 1), 1.0f, 0);


	mRenderDevice->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{

}


int main()
{
	TestApp* app = new TestApp();
	app->RunGame();
	return 0;
}