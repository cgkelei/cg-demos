#include "MainApp/Application.h"
#include "MainApp/Window.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Camera.h"
#include "Graphics/IRenderModule.h"
#include "Core/Exception.h"
#include "Core/Context.h"
#include "Core/XMLDom.h"
#include "Core/ModuleManager.h"


namespace RcEngine {

	using namespace Core;

	Application* Application::msAppliation = NULL;

	Application::Application( void )
		: mIsRunning(false)
	{
		msAppliation = this;
		mConfigFile = "Config.xml";

		Context::Initialize();
		ModuleManager::Initialize();

		Context::GetSingleton().SetApplication(this);
	}

	Application::~Application( void )
	{
		Safe_Delete(mCamera);
		ModuleManager::Finalize();
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
		while( msg.message != WM_QUIT)
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
		mTimer.Tick();

		if(!mAppPaused)
			Update(mTimer.GetDeltaTime());
		else
			::Sleep(50);

		Render();
	}


	void Application::Initialize()
	{
		// Read Configuration
		ReadConfiguration();

		// Read configuration
		CreateGameWindow();

		// Load Modules
		LoadAllModules();

		// Init Render Device
		InitializeDevice();

		mCamera = new Render::Camera();
	}

	void Application::CreateGameWindow( )
	{
		mMainWindow = new Window("RcEngine", mSettings);
		mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
		mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
		mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
		mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
		mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);

	}

	void Application::InitializeDevice()
	{
		mRenderDevice->Create();
		mRenderDevice->CreateRenderWindow(mSettings);
	}

	Render::Camera* Application::GetCamera()
	{
		return mCamera;
	}

	Render::RenderDevice* Application::GetRenderDevice()
	{
		return mRenderDevice;
	}


	void Application::LoadAllModules()
	{
		ModuleManager::GetSingleton().Load(MT_Render_OpengGL);
		Render::IRenderModule* renderModule = static_cast<Render::IRenderModule*>(
			ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL));
		renderModule->Initialise();
		mRenderDevice = renderModule->GetRenderDevice();
		Context::GetSingleton().SetRenderDevice(mRenderDevice);

	}

	void Application::UnloadAllModules()
	{
		ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL)->Shutdown();
	}


	void Application::Window_ApplicationActivated()
	{
		mActice = true;
		Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(true);
	}

	void Application::Window_ApplicationDeactivated()
	{
		mActice = false;
		Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(false);
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
		XMLDocument configXML;
		XMLNodePtr root = configXML.Parse("F://Config.xml");
		std::string windowTitle = root->Attribute("Title")->ValueString();

		XMLNodePtr graphics = root->FirstNode("Graphics");
		XMLNodePtr frame = graphics->FirstNode("Frame");


		mSettings.Left = 100;
		mSettings.Top = 100;
		mSettings.Width = frame->Attribute("Width")->ValueUInt();
		mSettings.Height = frame->Attribute("Height")->ValueUInt();
		mSettings.Fullscreen = (frame->Attribute("FullScreen")->ValueInt() == 1);
		mSettings.SampleCount = 0;
		mSettings.SampleQuality = 0;

		std::string colorFmt = frame->Attribute("ColorForamt")->ValueString();
		std::string depthStencilFmt = frame->Attribute("DepthStencilFormat")->ValueString();
		mSettings.ColorFormat = Render::PF_R8G8B8A8;
		mSettings.DepthStencilFormat = Render::PF_Depth24Stencil8;
	}

} // Namespace RcEngine