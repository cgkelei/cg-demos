#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Scene/SceneManager.h>
#include <Core/Exception.h>
#include <Core/Context.h>
#include <Core/XMLDom.h>
#include <Core/IModule.h>
#include <Core/ModuleManager.h>
#include <Resource/ResourceManager.h>
#include <Input/InputSystem.h>
#include <Input/InputDevice.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Texture.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Mesh.h>
#include <Graphics/Pipeline.h>

namespace RcEngine {

Application::Application( const String& config )
	: mIsRunning(false), mAppPaused(false), mConfigFile(config)
{
	Context::Initialize();
	ModuleManager::Initialize();
	FileSystem::Initialize();
	ResourceManager::Initialize();

	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Mesh, "Mesh", Mesh::FactoryFunc);
	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Material, "Material", Material::FactoryFunc);
	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Effect, "Effect", Effect::FactoryFunc);
	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Animation, "Animation",AnimationClip::FactoryFunc);
	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Texture, "Texture", TextureResource::FactoryFunc);
	ResourceManager::GetSingleton().RegisterType(ResourceTypes::Pipeline, "Pipeline", Pipeline::FactoryFunc);

	ReadConfiguration();

	Context::GetSingleton().SetApplication(this);
}

Application::~Application( void )
{
	ResourceManager::Finalize();
	ModuleManager::Finalize();
	Context::Finalize();
	FileSystem::Finalize();
}

void Application::RunGame()
{
	mIsRunning = true;

	Initialize();
	LoadContent();

	// Reset Game Timer
	mTimer.Reset();

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT && mIsRunning)
	{   
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );

		}
		else
		{
			Tick();
		}
	}

	UnloadContent();
}

void Application::Tick()
{
	InputSystem& inputSystem = Context::GetSingleton().GetInputSystem();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();

	mTimer.Tick();

	if (mActice)
	{
		inputSystem.Update(mTimer.GetDeltaTime());
		if (inputSystem.GetKeyboard()->KeyPress(KC_Escape))
		{
			mIsRunning  = false;
			return;
		}
	}

	if(!mAppPaused)
	{	
		Update(mTimer.GetDeltaTime());

		shared_ptr<FrameBuffer> currentFrameBuffer = renderDevice.GetCurrentFrameBuffer();
		//renderDevice.BindFrameBuffer(currentFrameBuffer);

		// update scene graph
		sceneMan.UpdateSceneGraph(mTimer.GetDeltaTime());
		sceneMan.UpdateRenderQueue(currentFrameBuffer->GetCamera());

		static int frameCount = 0;
		static float baseTime = 0;

		frameCount++;

		if (mTimer.GetGameTime()-baseTime >= 1.0f)
		{
			float fps = (float)frameCount;
			std::stringstream sss; 
			sss << "FPS: " << fps;
			mMainWindow->SetTitle(sss.str());
			frameCount = 0;
			baseTime += 1.0f;
		}
		

		float clr = (float)169/255;
		currentFrameBuffer->Clear(CF_Color | CF_Depth |CF_Stencil, RcEngine::ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);

		Context::GetSingleton().GetSceneManager().RenderScene();

		currentFrameBuffer->SwapBuffers();
	}
	else
		::Sleep(50);
	
}

void Application::LoadAllModules()
{
	ModuleManager::GetSingleton().Load(MT_Render_OpengGL);
	ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL)->Initialise();

	ModuleManager::GetSingleton().Load(MT_Input_OIS);
	ModuleManager::GetSingleton().GetMoudleByType(MT_Input_OIS)->Initialise();

	new SceneManager;
}

void Application::UnloadAllModules()
{
	ModuleManager::GetSingleton().UnloadAll();
}


void Application::Window_ApplicationActivated()
{
	mActice = true;
	Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(true);
}

void Application::Window_ApplicationDeactivated()
{
	mActice = false;
	Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(false);
}

void Application::Window_Suspend()
{
	mTimer.Stop();
}

void Application::Window_Resume()
{
	mTimer.Start();
}

void Application::Window_Paint()
{

}

void Application::ReadConfiguration()
{
	FileStream config;
	if ( !config.Open(mConfigFile) )
	{
		std::cout << "App config file " + mConfigFile + " doesn't found!" << std::endl;
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "App config file " + mConfigFile + " doesn't found!", "Application::ReadConfiguration");
	}

	
	XMLDoc xmlConfig;
	XMLNodePtr appNode = xmlConfig.Parse(config);

	mAppTitle = appNode->Attribute("Title")->ValueString();

	XMLNodePtr graphicNode = appNode->FirstNode("Graphics");

	mSettings.Left = graphicNode->Attribute("Left")->ValueInt();
	mSettings.Top = graphicNode->Attribute("Top")->ValueInt();
	mSettings.Width = graphicNode->Attribute("Width")->ValueUInt();
	mSettings.Height = graphicNode->Attribute("Height")->ValueUInt();
	mSettings.Fullscreen = graphicNode->Attribute("FullScreen")->ValueInt() != 0;
	mSettings.ColorFormat = PixelFormatUtils::GetPixelFormat(graphicNode->Attribute("ColorForamt")->ValueString());
	mSettings.DepthStencilFormat = PixelFormatUtils::GetPixelFormat(graphicNode->Attribute("DepthStencilFormat")->ValueString());

	XMLNodePtr sampleNode = graphicNode->FirstNode("Sample");
	if (sampleNode)
	{
		mSettings.SampleCount = sampleNode->Attribute("Count")->ValueUInt();
		mSettings.SampleQuality = sampleNode->Attribute("Quality")->ValueUInt();
	}

	XMLNodePtr syncNode = graphicNode->FirstNode("SyncInterval");
	if (syncNode)
	{
		mSettings.SyncInterval = syncNode->Attribute("Interval")->ValueUInt();
	}
	
	

	XMLNodePtr resNode = appNode->FirstNode("Resource");
	for (XMLNodePtr groupNode = resNode->FirstNode("Group"); groupNode; groupNode = groupNode->NextSibling("Group"))
	{
		String groupName = groupNode->Attribute("Name")->ValueString();
		for (XMLNodePtr pathNode = groupNode->FirstNode("Path"); pathNode; pathNode = pathNode->NextSibling("Path"))
		{
			String pathName = pathNode->Attribute("Name")->ValueString();
			ResourceManager::GetSingleton().AddResourceGroup(groupName);
			FileSystem::GetSingleton().RegisterPath(pathName, groupName);
		}
	}
}

void Application::Create()
{
	// Create main window
	mMainWindow = new Window("RcEngine", mSettings);
	mMainWindow->UserResizedEvent.bind(this, &Application::Window_UserResized);
	mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
	mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
	mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
	mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
	mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);

	// load all modules
	LoadAllModules();

	// Create render window
	Context::GetSingleton().GetRenderDevice().CreateRenderWindow(mSettings);

	// Show main window
	mMainWindow->ShowWindow();
}

void Application::Release()
{
	
}

void Application::Window_UserResized()
{
	mMainWindow->UpdateWindowSize();
	uint32_t width = mMainWindow->GetWidth();
	uint32_t height = mMainWindow->GetHeight();
	Context::GetSingleton().GetInputSystem().Resize(width, height);
	Context::GetSingleton().GetRenderDevice().Resize(width, height);	
}

} // Namespace RcEngine