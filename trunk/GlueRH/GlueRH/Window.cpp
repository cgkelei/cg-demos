#include "Window.h"
#include "Application.h"

namespace GlueRH
{
	Window* Window::m_pGlobalWindow = NULL;

	LRESULT CALLBACK GlobalWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		assert( MainWindow() );
		return MainWindow()->WndProc( hWnd, uMsg, wParam, lParam );
	}

	//////////////////////////////////////////////////////////////////////////
	Window::Window( const std::wstring& name, int left, int top, int32 width, int32 height, bool fullscreen )
		: mLeft(left), mTop(top), mWidth(width), mHeight(height),
		  mName(name), mFullscreen(fullscreen),
		  mHwnd(NULL)
	{
		if( m_pGlobalWindow )
		{
			throw std::exception("Window object already instantiated!\n" );
		}

		m_pGlobalWindow = this;
		mActive = false;
	}

	Window::~Window(void)
	{
		if (mHwnd != NULL)
		{
			DestroyWindow(mHwnd);
			mHwnd = NULL;
			m_pGlobalWindow = NULL;
		}
		
	}

	void Window::RegisterClass()
	{
		// Register the window class.
		WNDCLASSEX wc;
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= GlobalWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= AppInstance();
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= mName.c_str();
		wc.hIconSm			= NULL;

		RegisterClassEx(&wc);
	}

	void Window::InitInstance()
	{
		RECT rc = { 0, 0, mWidth, mHeight };

		uint32 style;
		if(mFullscreen)
			style = WS_POPUP;
		else
			style = WS_OVERLAPPEDWINDOW;

		AdjustWindowRect(&rc, style, false);


		// Create the application's window.
		mHwnd = CreateWindow( mName.c_str(), mName.c_str(), style,
			mLeft, mTop, rc.right - rc.left, rc.bottom - rc.top, 
			NULL, NULL, AppInstance(), NULL );

		GetClientRect(mHwnd, &rc);
		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

		ShowWindow(mHwnd, SW_SHOWNORMAL);
		UpdateWindow(mHwnd);
	}

	LRESULT Window::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch(uMsg)
		{
		case WM_ACTIVATE:
			{
				if (WA_INACTIVE == LOWORD(wParam))
				{
					OnActive()(false);
				}
				else
				{
					OnActive()(true);
				}
			}
			break;
		
		case WM_ERASEBKGND:
			return 1;
		
		case WM_PAINT:			OnPaint();
			break;
		case WM_MOVE:
			break;


		case WM_SIZE:
			{
				// Check to see if we are losing or gaining our window.  Set the
				// active flag to match
				if ((SIZE_MAXHIDE == wParam) || (SIZE_MINIMIZED == wParam))
					OnSize()(false);
				else
					OnSize()(true);
			}
			break;
		case WM_ENTERSIZEMOVE:
			// prevent rendering while moving / sizing
			OnEnterSizeMove()();
			break;

		case WM_EXITSIZEMOVE:			OnExitSizeMove()();
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 100;
			reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_SETCURSOR:			OnSetCursor()();
			break;
		case WM_CLOSE:
			{
				OnClose()();
				::PostQuitMessage(0);
				return 0;
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
