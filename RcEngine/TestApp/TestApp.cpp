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
#include "MainApp/Window.h"
#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include <D3DX10Math.h>

#pragma comment(lib, "D3DX10.lib")

using namespace RcEngine::Math;
using namespace RcEngine::Render;
using namespace RcEngine::Content;
using namespace RcEngine::Input;

TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

void TestApp::Initialize()
{
	Camera* camera = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	camera->SetViewParams(Vector3f(0, 10, 60), Vector3f(0, 10, 0), Vector3f(0, 1, 0));
	camera->SetProjectionParams(PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 1000.0f );
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Core::Context::GetSingleton().GetRenderDevice().GetRenderFactory();

	// Load mesh
	//RcEngine::FileStream modelSource("../Media/Mesh/Test/Test.mdl", RcEngine::FILE_READ);
	RcEngine::FileStream modelSource("../Media/Mesh/Dwarf/Dwarf.mdl", RcEngine::FILE_READ);
	mDwarf = Mesh::Load(modelSource);

	mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Dwarf/Body.material.xml");
	//
	//mDwarfMaterial = factory->CreateMaterialFromFile("Body", "../Media/Mesh/Test/Armor.material.xml");
	mDwarf->SetMaterial(mDwarfMaterial);
}

void TestApp::UnloadContent()
{
	
}



void TestApp::Render()
{
	RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();

	device->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
		RcEngine::Math::ColorRGBA(1.1f, 1.1f, 1.1f, 1.0f), 1.0f, 0);


	for (uint32_t i = 0; i < mDwarf->GetMeshPartCount(); ++i)
	{
		mDwarf->GetMeshPart(i)->Render();
	}

	device->GetCurrentFrameBuffer()->SwapBuffers();
}

void TestApp::Update( float deltaTime )
{
	static float degree = 0;
	degree += deltaTime * 0.001f ;

	Quaternionf quat = QuaternionFromRotationYawPitchRoll(0.0f, ToRadian(degree), 0.0f);
	mDwarf->SetWorldMatrix(/* QuaternionToRotationMatrix(quat)**/ CreateScaling(10.0f, 10.0f, 10.0f) );

	Keyboard* keyboard = static_cast<Keyboard*>( Context::GetSingleton().GetInputSystem().GetDevice(0) );
	Mouse* mouse =  static_cast<Mouse*>( Context::GetSingleton().GetInputSystem().GetDevice(1) );

	static float pos = 30;
	if(keyboard->KeyPress(KC_W))
	{	
		std::cout << "KC_W Pressed" << std::endl;
	}

	//if(keyboard->KeyDown(KC_S))
	//{
	//	std::cout << "KC_S Pressed" << std::endl;
	//}

	//if (mouse->ButtonDown(MS_LeftButton))
	//{
	//std::cout << "X=" << mouse->MouseMoveX() << " Y=" << mouse->MouseMoveY() << std::endl;
	//}

	//if (mouse->ButtonPress(MS_RightButton))
	//{
	//	std::cout << "X=" << mouse->X() << " Y=" << mouse->Y() << std::endl;
	//}


}

int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}