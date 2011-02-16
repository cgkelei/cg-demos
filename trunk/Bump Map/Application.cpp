#include "Application.h"
#include "Renderer.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "Camera.h"

#define CLASS_NAME "Class" 

Application* Application::ms_app = NULL;

//------------------------------------------------------------------------------
//Message Handle
LRESULT CALLBACK Application::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ms_app)
	{
		return ms_app->MyWndProc(hWnd, message, wParam,lParam);
	} 
	else
		return ::DefWindowProc(hWnd, message, wParam,lParam);
}

LRESULT CALLBACK Application::MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_SIZE:
		{
			if(m_renderer != NULL)
				OnSize(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
		}
		break;

	//Handle keyboard message
	case WM_KEYDOWN:
	case WM_KEYUP: 
		m_keyboardHandler->handleMessage(message,wParam, lParam);
		break;
	
	//Handle mouse message
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		m_mouseHandler->handleMessage(message,wParam, lParam);
		break;

	}

	return ::DefWindowProc(hWnd, message, wParam,lParam); 
}
//-----------------------------------------------------------------------------

Application::Application(const std::string& title, int32 width, int32 height)
	: m_width(width), m_height(height),m_hwnd(NULL),m_active(false), m_renderer(NULL)
{
	ms_app = this;
	m_title = title;
	m_hInst = ::GetModuleHandle(NULL);
	UNREFERENCED_PARAMETER( m_hInst );
	m_keyboardHandler = KeyboardHandler::Instance();
	m_mouseHandler = MouseHandler::Instance();
	CreateWin();
	CreateRenderer(m_hwnd, m_width, m_height);
	m_camera = new Camera;
	m_renderer->SetCamera(m_camera);
}

Application::~Application(void)
{
	if (m_renderer)
	{
		delete m_renderer;
		m_renderer = NULL;
	}
	if(m_camera)
	{
		delete m_camera;
		m_camera = NULL;
	}


	UnregisterClass(CLASS_NAME, m_hInst);
}

void Application::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while( msg.message != WM_QUIT)
	{			
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			m_renderer->beginFrame();
			m_renderer->drawFrame();
			m_renderer->endFrame();
		}
	}
}

void Application::CreateWin()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Application::WndProcStatic;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInst;
	wc.hIcon = NULL;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName	= NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx( &wc );

	RECT rc = { 0, 0, m_width, m_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindow( CLASS_NAME, m_title.c_str(), WS_OVERLAPPEDWINDOW,
		100, 100, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, m_hInst, NULL);

	::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hwnd);

	::SetFocus(m_hwnd);
}

void Application::CreateRenderer(HWND hwnd, int32 width, int height)
{
	m_renderer =  new Renderer(hwnd, width, height);
}

void Application::OnSize(const int width, const int height)
{
	m_width = width;
	m_height = height;
	m_renderer->OnSize(width, height);
	
}