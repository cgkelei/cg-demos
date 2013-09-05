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
#include "GUI/TextEdit.h"
#include "GUI/ComboBox.h"
#include "GUI/Slider.h"
#include "GUI/CheckBox.h"
#include "GUI/ScrollBar.h"
#include "GUI/Button.h"
#include "GUI/ListBox.h"
#include "GUI/LineEdit.h"
#include "GUI/UIWindow.h"
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

	resMan.AddResource(RT_Material, "Sprite.material.xml", "General");
	//resMan.AddResource(RT_Mesh, "him.mesh", "Custom");
	//resMan.AddResource(RT_Font, "Consolas Regular", "General");
	//resMan.LoadAllFromDisk();

	mSpriteBatch = std::make_shared<SpriteBatch>();

	// Entity
	Entity* dudeEntity = sceneManager->CreateEntity("Dude", "him.mesh",  "Custom");
	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(float3(0, 0, 0));
	dudeNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(180.0f), 0.0f, 0.0f));
	dudeNode->AttachObject(dudeEntity);
	//dudeEntity->SetVisible(false);

	AnimationPlayer* animPlayer = dudeEntity->GetAnimationPlayer();
	AnimationState* takeClip = animPlayer->GetClip("Take 001");
	takeClip->WrapMode = AnimationState::Wrap_Loop;

	animPlayer->PlayClip("Take 001");

	mTexture = factory->CreateTextureFromFile(FileSystem::GetSingleton().Locate("xWinForm.png"));
	
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
	
	//mSpriteBatch->Begin();

	//Rectanglef dest(100, 100, 400, 400);
	//IntRect src(94, 60, 1, 20);
	//mSpriteBatch->Draw(mTexture, dest, &src, ColorRGBA::White);
	////mSpriteBatch->Draw(mTexture, dest, nullptr, ColorRGBA::White);
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
	mFPSLabel->SetName("FPSLabel");
	mFPSLabel->SetPosition(int2(500, 500));
	mFPSLabel->SetSize(int2(300, 100));
	mFPSLabel->SetFont(UIManager::GetSingleton().GetDefaultFont(), 20.0f);
	rootElem->AddChild( mFPSLabel );	

	//mButton = new Button;
	//mButton->SetName("Button");
	//mButton->InitGuiStyle(nullptr);
	//mButton->SetPosition(int2(20, 30));
	//mButton->SetSize(int2(100, 40));
	//mButton->SetText(L"Button");
	//mButton->EventButtonClicked.bind(this, &TestApp::ButtonClicked);
	//rootElem->AddChild( mButton );	

	mCheckBox = new CheckBox;
	mCheckBox->SetName("CheckBox");
	mCheckBox->InitGuiStyle(nullptr);
	mCheckBox->SetPosition(int2(20, 60));
	mCheckBox->SetSize(int2(150, 16));
	mCheckBox->SetText(L"CheckBox");
	mCheckBox->EventStateChanged.bind(this, &TestApp::CheckBoxToggle);
	//rootElem->AddChild(mCheckBox);

	//mSlider = new Slider(UI_Horizontal);
	//mSlider->SetName("Slider");
	//mSlider->InitGuiStyle(nullptr);
	//mSlider->SetPosition(int2(20, 150));
	//mSlider->SetSize(int2(200, 23));
	//mSlider->SetValue(100);
	//mSlider->EventValueChanged.bind(this, &TestApp::SliderValueChange);
	//rootElem->AddChild( mSlider );	

	std::wfstream file(L"Config.xml");
	std::wstring text((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
	file.close();

	mTextEdit = new TextEdit();
	mTextEdit->InitGuiStyle(nullptr);
	mTextEdit->SetName("TextEdit");
	mTextEdit->SetPosition(int2(20, 130));
	mTextEdit->SetSize(int2(300, 300));
	//mTextEdit->SetPosition(int2(20, 50));
	//mTextEdit->SetSize(int2(750, 450));
	mTextEdit->SetText(text);
	//rootElem->AddChild(mTextEdit);

	//mComboBox = new ComboBox;
	//mComboBox->InitGuiStyle(nullptr);
	//mComboBox->SetName("ComboBox");
	//mComboBox->SetPosition(int2(20, 450));
	//mComboBox->SetSize(int2(200, 30));
	////mComboBox->SetDropHeight(60);
	//mComboBox->AddItem(L"Cryteck");
	//mComboBox->AddItem(L"HBAO");
	//mComboBox->AddItem(L"Unreal4");
	//mComboBox->AddItem(L"Alchemy");	
	//mComboBox->SetSelectedIndex(0);
	//rootElem->AddChild(mComboBox);

	mWindow1 = new UIWindow;
	mWindow1->InitGuiStyle(nullptr);
	mWindow1->SetName("Window0");
	mWindow1->SetTitle(L"Window Title");
	mWindow1->SetPosition(int2(20, 50));
	mWindow1->SetSize(int2(380, 450));
	rootElem->AddChild( mWindow1 );	

	mLineEdit = new LineEdit();
	mLineEdit->InitGuiStyle(nullptr);
	mLineEdit->SetName("LineEdit");
	mLineEdit->SetPosition(int2(150, 60));
	mLineEdit->SetSize(int2(180, 40));
	mLineEdit->SetText(L"mLineEdit = new TextEdit(TextEdit::LineEdit);");
	mWindow1->AddChild(mLineEdit);
	
	mWindow1->AddChild(mTextEdit);
	mWindow1->AddChild(mCheckBox);
	mWindow1->AddChild(mLineEdit);

	mWindow2 = new UIWindow;
	mWindow2->InitGuiStyle(nullptr);
	mWindow2->SetName("Window1");
	mWindow2->SetTitle(L"Window Title");
	mWindow2->SetPosition(int2(420, 100));
	mWindow2->SetSize(int2(350, 350));
	rootElem->AddChild( mWindow2 );	

	mListBox = new ListBox;
	mListBox->InitGuiStyle(nullptr);
	mListBox->SetName("ListBox");
	mListBox->SetPosition(int2(20, 50));
	mListBox->SetSize(int2(200, 90));
	mListBox->AddItem(L"ListBox");
	mListBox->AddItem(L"HBAO");
	mListBox->AddItem(L"Unreal4");
	mListBox->AddItem(L"Alchemy");	
	mListBox->SetSelectedIndex(1);
	mWindow2->AddChild(mListBox);
}

void TestApp::DrawUI()
{
	wchar_t buffer[100];
	int cx = swprintf ( buffer, 100, L"Graphics Demo FPS: %d", mFramePerSecond );
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