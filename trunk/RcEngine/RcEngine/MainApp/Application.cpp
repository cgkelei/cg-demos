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
#include <Input/InputSystem.h>

namespace RcEngine {

Application::Application( void )
	: mIsRunning(false), mAppPaused(false)
{
	mConfigFile = "Config.xml";
	Context::Initialize();
	ModuleManager::Initialize();

	Context::GetSingleton().SetApplication(this);
}

Application::~Application( void )
{
	ModuleManager::Finalize();
	Context::Finalize();
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
	if (mActice)
	{
	Context::GetSingleton().GetInputSystem().Update(mTimer.GetDeltaTime());
	}

	mTimer.Tick();

	if(!mAppPaused)
	{
		Update(mTimer.GetDeltaTime());
		Context::GetSingleton().GetSceneManager().UpdateSceneGraph();
	}
	else
		::Sleep(50);
	
	static int frameCount = 0;
	static float baseTime = 0;

	frameCount++;

	if (mTimer.GetGameTime()-baseTime >= 1.0f)
	{
		float fps = (float)frameCount;
		std::stringstream sss; 
		sss << "FPS: " << fps;
		mMainWindow->SetTitle(sss.str());
		//fprintf(stdout, "FPS: %4.2f\n", fps);
		frameCount = 0;
		baseTime += 1.0f;
	}


	/*Render();	*/
	
	
	RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();
	Camera* cam = device->GetCurrentFrameBuffer()->GetCamera();
	Context::GetSingleton().GetSceneManager().UpdateRenderQueue(cam);


	shared_ptr<FrameBuffer> currentFrameBuffer = device->GetCurrentFrameBuffer();
	device->BindFrameBuffer(currentFrameBuffer);

	float clr = (float)169/255;
	currentFrameBuffer->Clear(CF_Color | CF_Depth |CF_Stencil, RcEngine::ColorRGBA(clr, clr, clr, 1.0f), 1.0f, 0);
	
	Context::GetSingleton().GetSceneManager().RenderScene();

	currentFrameBuffer->SwapBuffers();
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
	mSettings.Left = 40;
	mSettings.Top = 40;
	mSettings.Width = 800;
	mSettings.Height = 600;
	mSettings.Fullscreen = false;
	mSettings.SampleCount = 0;
	mSettings.SampleQuality = 0;
	mSettings.ColorFormat = PF_R8G8B8A8;
	mSettings.DepthStencilFormat = PF_Depth24Stencil8;
	mSettings.SyncInterval = 0;

}

void Application::Create()
{
	// todo move this to context
	ReadConfiguration();

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