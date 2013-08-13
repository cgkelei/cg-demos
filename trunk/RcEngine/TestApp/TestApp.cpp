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
#include "GUI/UIManager.h"
#include "GUI/Label.h"
#include "GUI/Slider.h"
#include "GUI/CheckBox.h"
#include "GUI/ScrollBar.h"
#include "GUI/Button.h"
#include "Core/Variant.h"
#include "Core/StringHash.h"

TestApp::TestApp( const String& config )
	:Application(config), mFramePerSecond(0)
{
	//Variant var(true);

	//VariantMap param;

	//param[StringHash("Test")] = var;
}


TestApp::~TestApp(void)
{
	delete mCameraControler;
}

void TestApp::Initialize()
{
			printf("Ok\n");

	InitGUI();

	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	
	float3 up(0, 1, 0);
	camera->SetViewParams(float3(0, 50, 150), float3(0, 50, 0), up);
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
//	dudeNode->SetPosition(float3(0, 0, 0));
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

	/*resMan.AddResource(RT_Material, "Sprite.material.xml", "General");
	resMan.AddResource(RT_Mesh, "him.mesh", "Custom");*/
	resMan.AddResource(RT_Font, "Consolas Regular", "General");
	resMan.LoadAllFromDisk();

	// Font Sprite 
	mSpriteBatchFont = std::make_shared<SpriteBatch>(
		std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "Font.material.xml", "General")));
	
	// Font
	mFont = std::static_pointer_cast<Font>(resMan.GetResourceByName(RT_Font,"Consolas Regular", "General"));

	//auto ZY = mFont->GetGlyphInfo(L'Z').SrcY;
	//auto AY = mFont->GetGlyphInfo(L'A').SrcY;

	mSpriteBatch = std::make_shared<SpriteBatch>();
	
	// Entity
	Entity* dudeEntity = sceneManager->CreateEntity("Dude", "him.mesh",  "Custom");
	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(float3(0, 0, 0));
	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
	dudeNode->AttachObject(dudeEntity);


	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->WrapMode = AnimationState::Wrap_Loop;

	animPlayer->PlayClip("Take 001");

	mTexture = factory->CreateTextureFromFile(FileSystem::GetSingleton().Locate("Glass.dds"));

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

	shared_ptr<FrameBuffer> currentFrameBuffer = renderDevice.GetCurrentFrameBuffer();

	renderDevice.BindFrameBuffer(currentFrameBuffer);

	float clr = (float)169/255;
	currentFrameBuffer->Clear(CF_Color | CF_Depth |CF_Stencil, RcEngine::ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);
	
	//Rectanglef region(100, 100, 300, 300);

	//mSpriteBatchFont->Begin();

	//wchar_t buffer[100];
	//int cx = swprintf ( buffer, 100, L"FPS: %d", mFramePerSecond );
	//mFont->DrawString(*mSpriteBatchFont, std::wstring(buffer, cx), 30, float2(20, mMainWindow->GetHeight() - 30), ColorRGBA(1, 0, 0, 1));

	//std::wstring wstr = L"mFont->DrawString(*mSpriteBatch,\n std::wstring(buffer, cx), 30,\n float2(20, 580), ColorRGBA(0, 0, 0, 1));";
	//
	////mFont->DrawStringWrap(*mSpriteBatch, wstr, 30, 500, float2(20, 200), ColorRGBA(0, 0, 0, 1));
	////mFont->DrawString(*mSpriteBatchFont, wstr, 15, Font::AlignLeft | Font::AlignTop, region, ColorRGBA(0, 0, 0, 1));

	//mSpriteBatchFont->End();
	//mSpriteBatchFont->Flush();

	//mSpriteBatch->Begin();
	//mSpriteBatch->Draw(mTexture, region, ColorRGBA(1, 1, 1, 1));
	//mSpriteBatch->End();
	//mSpriteBatch->Flush();

	// todo 
	// Move to engine level
	scenenMan.UpdateRenderQueue(currentFrameBuffer->GetCamera(), RO_StateChange);
	scenenMan.RenderScene();

	DrawUI();

	// Swap Buffer
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

void TestApp::InitGUI()
{
	UIElement* rootElem = UIManager::GetSingleton().GetRoot();

	mFPSLabel = new Label();
	mFPSLabel->SetPosition(int2(20, 100));
	mFPSLabel->SetSize(int2(100, 40));
	mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20);
	rootElem->AddChild( mFPSLabel );	

	mButton = new Button;
	mButton->SetPosition(int2(20, 150));
	mButton->SetSize(int2(100, 40));
	mButton->SetText(L"Button");
	mButton->EventButtonClicked.bind(this, &TestApp::ButtonClicked);
	rootElem->AddChild( mButton );	

	mCheckBox = new CheckBox;
	mCheckBox->SetPosition(int2(20, 200));
	mCheckBox->SetSize(int2(150, 27));
	mCheckBox->SetText(L"CheckBox");
	mCheckBox->EventStateChange.bind(this, &TestApp::CheckBoxToggle);
	rootElem->AddChild(mCheckBox);

	mSlider = new Slider();
	mSlider->SetOrientation(UI_Horizontal);
	mSlider->SetPosition(int2(20, 250));
	mSlider->SetSize(int2(200, 23));
	mSlider->SetValue(100);
	mSlider->EventValueChange.bind(this, &TestApp::SliderValueChange);
	rootElem->AddChild( mSlider );	
}

void TestApp::DrawUI()
{
	wchar_t buffer[100];
	int cx = swprintf ( buffer, 100, L"FPS: %d", mFramePerSecond );
	mFPSLabel->SetText(buffer);

	// Render UI
	UIManager::GetSingleton().Render();
}

void TestApp::CalculateFrameRate()
{
	static int frameCount = 0;
	static float baseTime = 0;

	frameCount++;

	if (mTimer.GetGameTime()-baseTime >= 1.0f)
	{
		mFramePerSecond = frameCount;
		frameCount = 0;
		baseTime += 1.0f;
	}
}

void TestApp::SliderValueChange( int32_t value )
{
	printf("Slider value: %d\n", value);
}

void TestApp::ButtonClicked()
{
	printf("Button: Clicked\n");
}

void TestApp::CheckBoxToggle( bool checked )
{
	if (checked)
		printf("CheckBox: checked\n");
	else
		printf("CheckBox: unchecked\n");
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