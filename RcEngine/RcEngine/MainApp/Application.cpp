#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Texture.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Mesh.h>
#include <Graphics/Pipeline.h>
#include <Graphics/Font.h>
#include <Scene/SceneManager.h>
#include <Core/Exception.h>
#include <Core/Context.h>
#include <Core/XMLDom.h>
#include <Core/IModule.h>
#include <Core/ModuleManager.h>
#include <Resource/ResourceManager.h>
#include <Input/InputSystem.h>
#include <Input/InputEvent.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <GUI/UIManager.h>

// C++ 11 thread
#include <thread>

namespace RcEngine {

Application::Application( const String& config )
	: mEndGame(false), mAppPaused(false), mConfigFile(config)
{
	Context::Initialize();
	ModuleManager::Initialize();
	FileSystem::Initialize();
	ResourceManager::Initialize();
	InputSystem::Initialize();
	UIManager::Initialize();

	// todo add sub scene manager
	new SceneManager;

	ResourceManager& resMan = ResourceManager::GetSingleton();
	resMan.RegisterType(RT_Mesh, "Mesh", Mesh::FactoryFunc);
	resMan.RegisterType(RT_Material, "Material", Material::FactoryFunc);
	resMan.RegisterType(RT_Effect, "Effect", Effect::FactoryFunc);
	resMan.RegisterType(RT_Animation, "Animation",AnimationClip::FactoryFunc);
	resMan.RegisterType(RT_Texture, "Texture", TextureResource::FactoryFunc);
	resMan.RegisterType(RT_Pipeline, "Pipeline", Pipeline::FactoryFunc);
	resMan.RegisterType(RT_Font, "Font", Font::FactoryFunc);

	ReadConfiguration();

	Context::GetSingleton().SetApplication(this);
}

Application::~Application( void )
{
	// todo, move to dll unload
	//delete Context::GetSingleton().GetSceneManagerPtr();

	//ResourceManager::Finalize();
	//ModuleManager::Finalize();
	//FileSystem::Finalize();
	//InputSystem::Finalize();
	//UIManager::Finalize();
	//Context::Finalize();
}

void Application::RunGame()
{
	Initialize();

	LoadContent();

	mTimer.Reset();

	int i = 0;
	do 
	{
		Tick();

		//i++;
		//if (i==5)
		//	mEndGame = true;
	} while ( !mEndGame );

	UnloadContent();
}

void Application::Tick()
{
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& renderDevice = Context::GetSingleton().GetRenderDevice();
	InputSystem& inputSystem = InputSystem::GetSingleton();

	if (!mActice)
	{
		// Inative, Sleep for a while
		static const std::chrono::milliseconds inactiveSleepTime( 20 );
		std::this_thread::sleep_for( inactiveSleepTime );
	}

	// Andvance Game Time
	mTimer.Tick();

	// Read Input
	inputSystem.BeginEvents();
		mMainWindow->CollectOSEvents();
	inputSystem.EndEvents();

	float deltaTime = mTimer.GetDeltaTime();

	// Process input events
	ProcessEventQueue();

	inputSystem.Dispatch(deltaTime);

	// update
	Update(deltaTime);
	
	// update scene graph
	sceneMan.UpdateSceneGraph(deltaTime);

	// Update UI
	UIManager::GetSingleton().Update(deltaTime);

	// render
	Render();
}

void Application::ProcessEventQueue()
{
	InputSystem& inputSystem = InputSystem::GetSingleton();

	InputEvent event;
	while(inputSystem.PollEvent(&event))
	{
		bool eventConsumed = false;

		eventConsumed = UIManager::GetSingleton().OnEvent(event);
		
		if (!eventConsumed)
		{
			/*if (!mAppPaused)
			eventConsumed = mAppPaused*/

			if (!eventConsumed)
			{
				
				if (event.EventType == InputEventType::KeyDown && event.Key.key == KC_Escape)
				{
					eventConsumed = true;
					mEndGame = true;
					return;
				}
			}
		}
	}

}

void Application::LoadAllModules()
{
	ModuleManager::GetSingleton().Load(MT_Render_OpengGL);
	ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL)->Initialise();
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
	mAppPaused = true;
	mTimer.Stop();
}

void Application::Window_Resume()
{
	mAppPaused = true;
	mTimer.Start();
}

void Application::Window_Paint()
{

}

void Application::Window_UserResized()
{
	uint32_t width = mMainWindow->GetWidth();
	uint32_t height = mMainWindow->GetHeight();

	if (mSettings.Width != width || mSettings.Height != height)
	{
		Context::GetSingleton().GetRenderDevice().Resize(width, height);	
		UIManager::GetSingleton().OnWindowResize(width, height);
		//Context::GetSingleton().GetInputSystem().Resize(width, height);

		WindowResize(width, height);
	}	
}

void Application::Window_Close()
{
	mEndGame = true;
}

void Application::Create()
{
	// Create main window
	mMainWindow = new Window(mAppTitle, mSettings);
	mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
	mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
	mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
	mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
	mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);
	mMainWindow->WindowClose.bind(this, &Application::Window_Close);
	mMainWindow->UserResizedEvent.bind(this, &Application::Window_UserResized);

	// load all modules
	LoadAllModules();

	// Create render window
	Context::GetSingleton().GetRenderDevice().CreateRenderWindow(mSettings);

	// UI Graphics initailize
	UIManager::GetSingleton().OnGraphicsInitialize();

	// Show main window
	mMainWindow->ShowWindow();
}

void Application::Release()
{
	// Delete Scene Manager
	SceneManager* pSceneMan = Context::GetSingleton().GetSceneManagerPtr();
	delete pSceneMan;

	UIManager::Finalize();
	InputSystem::Finalize();
	FileSystem::Finalize();
	ResourceManager::Finalize();
	
	Context::Finalize();

	//ModuleManager::Finalize();	
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


} // Namespace RcEngine