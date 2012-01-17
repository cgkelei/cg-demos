#include "MainApp/Application.h"
#include "MainApp/Window.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Camera.h"
#include "Core/Exception.h"


namespace RcEngine {

using namespace Core;


Application* Application::msAppliation = NULL;

Application::Application( void )
	: mIsRunning(false)
{
	msAppliation = this;
	mConfigFile = "Config.xml";
}

Application::~Application( void )
{
	//Safe_Delete(mCamera);
	//Safe_Delete(mRenderFactory);
	//Safe_Delete(mRenderDevice);
}

void Application::RunGame()
{
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
	//mRenderDevice = new OpenGLRenderDevice();
	//mRenderFactory = new OpenGLRenderFactory();
}

RenderSystem::Camera* Application::GetCamera()
{
	return mCamera;
}

RenderSystem::RenderDevice* Application::GetRenderDevice()
{
	return mRenderDevice;
}

RenderSystem::RenderFactory* Application::GetRenderFactory()
{
	return mRenderFactory;
}

void Application::Window_ApplicationActivated()
{

}

void Application::Window_ApplicationDeactivated()
{

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