#include "Application.h"
#include "Window.h"

namespace GlueRH
{
	Application* Application::m_pGlobalApp = NULL;
	
	HINSTANCE g_hInstance;
	
	HINSTANCE AppInstance()
	{
		return g_hInstance;
	}

	

//////////////////////////////////////////////////////////////////////////
	Application::Application(void)
	{
		if( m_pGlobalApp )
		{
			throw std::exception("Application object already created!\n");
		}
		m_pGlobalApp = this;

		mTitle = std::wstring(L"Default Window Name");
		mActive = true;
	}


	Application::~Application(void)
	{
		
	}

	void Application::Run()
	{
		static float lastTime = (float)timeGetTime(); 

		bool gotMsg = false;
		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );

		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		while( msg.message != WM_QUIT)
		{	
			if(mActive)
			{
				gotMsg = ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0 );
			}
			else
			{
				gotMsg = ( GetMessage(&msg, NULL, 0, 0) != 0 );
			}

			if(gotMsg)
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				float currTime = (float)timeGetTime();
				float delta = (currTime - lastTime)/1000.f;

				if(mActive)
				{
					UpdateScene(delta);
					DrawScene(delta);
				}else
				{
					DoIdle(delta);
				}

				lastTime = currTime;
			}
		}
	}

	void Application::InitMainWindow()
	{
		new Window(mTitle, 0, 0, 640, 480);
		MainWindow()->InitInstance();
	}

	void Application::OnResize( int32 width, int32 height )
	{

	}

	void Application::UpdateScene( float timeDelta )
	{

	}

	void Application::DrawScene( float timeDelta )
	{

	}

	void Application::DoIdle( float timeDelta )
	{

	}

	void Application::SceneInit()
	{

	}

	void Application::SceneEnd()
	{

	}

	void Application::InitRenderDevice()
	{

	}

	void Application::Init()
	{

	}

	

	

	

}
