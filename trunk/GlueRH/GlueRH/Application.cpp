#include "Application.h"
#include "Window.h"

namespace GlueRH
{
	Application* Application::m_pGlobalApp = NULL;
	
	HINSTANCE g_hInstance;
	

//////////////////////////////////////////////////////////////////////////
	Application::Application(const std::wstring&  name)
		: mTitle(name)
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
				
				if(mActive)
				{	

				}else
				{	

				}

				
			}
		}
	}

	void Application::InitMainWindow()
	{
		new Window(mTitle, 0, 0, 640, 480);
		MainWindow()->InitWindow();
		MainWindow()->OnActive().bind(this, &Application::OnActived );
	}

	void Application::OnResize( int32 width, int32 height )
	{

	}

	void Application::OnActived ( bool active )
	{
		mActive = active;
	}
	
	void Application::OnPaint()
	{
	}

	void Application::InitRenderDevice()
	{

	}


	

	

}
