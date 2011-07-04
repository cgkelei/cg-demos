#include "Window.h"
#include "Application.h"

namespace GlueRH
{
	Window* Window::m_pGlobalWindow = NULL;

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

		mHinst = GetModuleHandle (0);
		m_pGlobalWindow = this;
		mActive = false;
	}

	Window::~Window(void)
	{
		if (mHwnd != NULL)
		{
			DestroyWindow(mHwnd);
		}
		if(m_pGlobalWindow)
		{
			m_pGlobalWindow = NULL;
		}
		
	}

	void Window::MyRegisterClass()
	{
		// Register the window class.
		WNDCLASSEX wc;
		wc.cbSize			= sizeof(wc);
		wc.style			= CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc		= Window::WndProcStatic;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= mHinst;
		wc.hIcon			= NULL;
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= mName.c_str();
		wc.hIconSm			= NULL;

		RegisterClassEx(&wc);
	}

	bool Window::InitInstance()
	{
		RECT rc = { 0, 0, mWidth, mHeight };

		uint32 style;
		if(mFullscreen)
			style = WS_POPUP;
		else
			style = WS_OVERLAPPEDWINDOW;

		AdjustWindowRect(&rc, style, false);


		// Create the application's window.
		mHwnd = ::CreateWindow( mName.c_str(), mName.c_str(), style,
			mLeft, mTop, rc.right - rc.left, rc.bottom - rc.top,
			NULL, NULL, mHinst, NULL );

		if( !mHwnd )
			return false;

		GetClientRect(mHwnd, &rc);
		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

		ShowWindow(mHwnd, SW_SHOWNORMAL);
		UpdateWindow(mHwnd);

		return true;
	}
	
	void Window::LoadWindow()
	{
		MyRegisterClass();
		assert( InitInstance() );
	}

	void Window::CloseWindow()
	{
		DestroyWindow(mHwnd);
		mHwnd = NULL;
	}

	void Window::OnUserResized()
	{
		if ( !UserResized.empty() )
		{
			UserResized();
		}
	}

	void Window::OnSuspend()
	{
		if ( !Suspend.empty() )
		{
			Suspend();
		}
	}

	void Window::OnResume()
	{
		if ( !Resume.empty() )
		{
			Resume();
		}
	}

	void Window::OnPaint()
	{
		if ( !Paint.empty() )
		{
			Paint();
		}
	}
	
	void Window::OnMonitorChanged()
	{
		if ( !MonitorChanged.empty() )
		{
			MonitorChanged();
		}
	}

	void Window::OnApplicationActivated()
	{
		if ( !ApplicationActivated.empty() )
		{
			ApplicationActivated();
		}
	}

	void Window::OnApplicationDeactivated()
	{
		if ( !ApplicationDeactivated.empty() )
		{
			ApplicationDeactivated();
		}
	}

	void Window::OnSystemSuspend()
	{
		if ( !SystemSuspend.empty() )
		{
			SystemSuspend();
		}
	}

	void Window::OnSystemResume()
	{
		if ( !SystemResume.empty() )
		{
			SystemResume();
		}
	}

	void Window::OnScreensaver( bool* cancel )
	{
		if ( !Screensaver.empty() )
		{
			Screensaver(cancel);
		}
	}


	Window::Size Window::GetCurrentSize() const
	{
		RECT rect;
		if (!GetClientRect(mHwnd, &rect))
		{
			rect.left = 0; rect.right = 0;
			rect.bottom = 0;
			rect.top = 0;
		}
		return std::make_pair(rect.right - rect.left, rect.bottom - rect.top);
	}


	void Window::UpdateMonitor()
	{
		HMONITOR windowMonitor = MonitorFromWindow(mHwnd, MONITOR_DEFAULTTOPRIMARY);
		if ( !mCurrentMonitor || windowMonitor != mCurrentMonitor )
		{
			mCurrentMonitor = windowMonitor;
			if (mCurrentMonitor)
				OnMonitorChanged();
		}
	}
	

	LRESULT CALLBACK Window::WndProcStatic( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		assert( m_pGlobalWindow );
		return m_pGlobalWindow->WndProc( hWnd, uMsg, wParam, lParam );
	}


	LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch (uMsg)
		{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
			{
				mIsMinimized = true;
				mIsMaximized = false;

				OnSuspend();
			}
			else
			{
				RECT rect;
				if (!GetClientRect(hWnd, &rect))
				{
					rect.left = 0; rect.right = 0;
					rect.bottom = 0;
					rect.top = 0;
				}

				if (rect.bottom - rect.top == 0)
				{
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					mIsMinimized = false;
					mIsMaximized = true;

					OnUserResized();
					UpdateMonitor();

				}
				else if (wParam == SIZE_RESTORED)
				{
					if (mIsMinimized)
						OnResume();

					mIsMinimized = false;
					mIsMaximized = false;

					Size newSize = GetCurrentSize();
					if ( !mIsInSizeMove && mCachedSize != newSize)
					{
						OnUserResized();
						UpdateMonitor();
						mCachedSize = newSize;
					}
				}
			}
			break;

		case WM_MOVE:
			break;

		case WM_PAINT:
			OnPaint();
			break;

		case WM_ACTIVATEAPP:
			if (wParam)
			{
				OnApplicationActivated();
			}
			else
			{
				OnApplicationDeactivated();
			}
			break;

		case WM_POWERBROADCAST:
			if (wParam == PBT_APMQUERYSUSPEND)
			{
				OnSystemSuspend();
				return 1;
			}
			else if (wParam == PBT_APMRESUMESUSPEND)
			{
				OnSystemResume();
				return 1;
			}
			break;

		case WM_SYSCOMMAND:
			{
				long wp = wParam & 0xFFF0;
				if (wp == SC_MONITORPOWER || wp == SC_SCREENSAVE)
				{
					bool cancel;
					OnScreensaver(&cancel);
					if (cancel)
					{
						return 0;
					}
				}
				break;
			}

		case WM_ENTERSIZEMOVE:
			mIsInSizeMove = true;
			mCachedSize = GetCurrentSize();
			OnSuspend();
			break;

		case WM_EXITSIZEMOVE:
			// check for screen and size changes
			OnUserResized();
			UpdateMonitor();
			mIsInSizeMove = false;

			// resume application processing
			OnResume();
			break;

		case WM_DESTROY:
			UnregisterClass( mName.c_str(), mHinst );
			PostQuitMessage(0);
			break;
	
		case WM_MOUSEWHEEL:
			mMouseWheel += (int16) (wParam >> 0x10);
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	

}
