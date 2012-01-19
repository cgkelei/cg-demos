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
#include "Core/ModuleManager.h"


namespace RcEngine {

	using namespace Core;

	Application* Application::msAppliation = NULL;

	Application::Application( void )
		: mIsRunning(false)
	{
		msAppliation = this;
		mConfigFile = "Config.xml";

		ModuleManager::Initialize();
	}

	Application::~Application( void )
	{
		//Safe_Delete(mCamera);
		ModuleManager::Finalize();
	}

	void Application::RunGame()
	{
		LoadAllModules();
		Initialize();

		mIsRunning = true;

		LoadContent();

		//// Reset Game Timer
		mTimer.Reset();

		while( mIsRunning )
		{
			mTimer.Tick();

			/*if(!mAppPaused)
			Update(mTimer.GetDeltaTime());
			else
			::Sleep(50);

			Render();

			mIsRunning = !glfwGetKey( GLFW_KEY_ESC ) &&
			glfwGetWindowParam( GLFW_OPENED );*/
		}

		UnloadContent();

		Finalize();
	}

	void Application::Initialize()
	{
		// Read configuration
		CreateGameWindow();

		InitRenderDevice();

		// Init Glew
		//GLenum err = glewInit();
		//if (GLEW_OK != err)
		//{
		//	char errMsg[1024];
		//	sprintf_s(errMsg, "Error initializing Glew: %s", glewGetErrorString(err));
		//	ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, errMsg, "Application::Initialize" );
		//}

		//// Init Camera
		//mCamera = new Camera();
	}

	void Application::Finalize()
	{
		//Safe_Delete(mCamera);
	}

	void Application::CreateGameWindow( )
	{
		/*XMLDocument configXML;
		XMLNodePtr root = configXML.Parse(mConfigFile);
		std::string windowTitle = root->Attribute("Title")->ValueString();

		XMLNodePtr graphics = root->FirstNode("Graphics");
		XMLNodePtr frame = graphics->FirstNode("Frame");


		mSettings.Width = frame->Attribute("Width")->ValueUInt();
		mSettings.Height = frame->Attribute("Height")->ValueUInt();
		mSettings.Fullscreen = (frame->Attribute("FullScreen")->ValueInt() == 1);

		std::string colorFmt = frame->Attribute("ColorForamt")->ValueString();
		std::string depthStencilFmt = frame->Attribute("DepthStencilFormat")->ValueString();

		mMainWindow = new Window(windowTitle, mSettings);
		mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
		mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
		mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
		mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
		mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);*/

	}

	void Application::InitRenderDevice()
	{
	}

	RenderSystem::Camera* Application::GetCamera()
	{
		return mCamera;
	}

	RenderSystem::RenderDevice* Application::GetRenderDevice()
	{
		return mRenderDevice;
	}


	void Application::LoadAllModules()
	{
		ModuleManager::GetSingleton().Load(MT_Render_OpengGL);
		RenderSystem::IRenderModule* renderModule = static_cast<RenderSystem::IRenderModule*>(
			ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL));
		renderModule->Install();
		mRenderDevice = renderModule->GetRenderDevice();
	}

	void Application::UnloadAllModules()
	{

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

} // Namespace RcEngine