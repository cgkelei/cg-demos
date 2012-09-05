#include <MainApp/Window.h>
#include <Core/Utility.h>

namespace RcEngine {

Window* Window::msWindow = 0;

LRESULT CALLBACK Window::WndProcStatic( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (msWindow)
	{
		return msWindow->WndProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


Window::Window( const std::string& title, const RenderSettings& settings )
	: mName(title), mInSizeMove(false), mMaximized(false), mMinimized(false)
{
	msWindow = this;
	mhInstance	= GetModuleHandle(NULL);

	WNDCLASSEX	wcex;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	mFullscreen = settings.Fullscreen;

	std::wstring wtitle;
	Convert(wtitle, title);
	
	wcex.cbSize			    = sizeof(WNDCLASSEX);
	wcex.style              = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc        = (Window::WndProcStatic);
	wcex.cbClsExtra         = 0;
	wcex.cbWndExtra         = 0;
	wcex.hInstance          = mhInstance;
	wcex.hIcon              = NULL;
	wcex.hCursor            = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground      = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName       = 0;
	wcex.lpszClassName      = wtitle.c_str();
	wcex.hIconSm            = NULL;

	if (!RegisterClassEx(&wcex))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,TEXT("Failed To Register The Window Class."), TEXT("ERROR"),  MB_OK|MB_ICONEXCLAMATION);
	}

	if (settings.Fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		::ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	WindowRect.left = (long)settings.Left;
	WindowRect.top=(long)settings.Top;		
	WindowRect.right=(long)settings.Width+WindowRect.left ;		
	WindowRect.bottom=(long)settings.Height+WindowRect.top;

	::AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size


	// Create The Window
	if (!(mhWnd = ::CreateWindowEx(dwExStyle,							// Extended Style For The Window
		wtitle.c_str(),							// Class Name
		wtitle.c_str(),						// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		WindowRect.left, WindowRect.top,	// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		mhInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		MessageBox(NULL,TEXT("Window Creation Error."),TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);
	}


	::GetClientRect(mhWnd, &WindowRect);
	mLeft = settings.Left;
	mTop = settings.Top;
	mWidth = WindowRect.right - WindowRect.left;
	mHeight = WindowRect.bottom - WindowRect.top;
}

Window::~Window(void)
{
}


void Window::ShowWindow()
{
	::ShowWindow(mhWnd, SW_SHOWNORMAL);
	::UpdateWindow(mhWnd);
}

void Window::Reposition( int32_t left, int32_t top )
{
	if(mhWnd && !mFullscreen)
	{
		mLeft = left;
		mTop = top;
		::SetWindowPos(mhWnd, 0, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void Window::OnUserResized()
{
	if(!UserResizedEvent.empty())
		UserResizedEvent();
}

void Window::OnSuspend()
{
	if(!SuspendEvent.empty())
		SuspendEvent();
}

void Window::OnResume()
{
	if(!ResumeEvent.empty())
		ResumeEvent();
}

void Window::OnApplicationActivated()
{
	if(!ApplicationActivatedEvent.empty())
		ApplicationActivatedEvent();
}

void Window::OnApplicationDeactivated()
{
	if(!ApplicationDeactivatedEvent.empty())
		ApplicationDeactivatedEvent();
}


void Window::OnPaint()
{
	if(!PaintEvent.empty())
		PaintEvent();
}

LRESULT Window::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mMinimized = true;
				mMaximized = false;
				OnSuspend();
			}
			else if(wParam == SIZE_MAXIMIZED)
			{
				mMinimized = false;
				mMaximized = true;
				OnUserResized();
			}
			else
			{
				if (mMinimized)
					OnResume();

				mMinimized = true;
				mMaximized = true;

				if(!mInSizeMove)
					OnUserResized();
			}
		}
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

	case WM_ENTERSIZEMOVE:
		{
			OnSuspend();
			mInSizeMove = true;
			//OnUserResized();
		}

		break;

	case WM_EXITSIZEMOVE:
		{
			OnUserResized();
			mInSizeMove = false;
			OnResume();
		}
		break;

	case WM_DESTROY:
		std::wstring wtitle;
		Convert(wtitle, mName);
		UnregisterClass(wtitle.c_str(), mhInstance);

		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Window::SetTitle( const String& title )
{
	std::wstring text;
	Convert(text, title);
	::SetWindowText(mhWnd, text.c_str());
}

void Window::UpdateWindowSize()
{
	RECT rect;
	::GetWindowRect(mhWnd, &rect);

	int32_t newLeft = rect.left;
	int32_t newTop = rect.top;

	if ((newLeft != mLeft) || (newTop != mTop))
	{
		mLeft = newLeft;
		mTop = newTop;
		Reposition(newLeft, newTop);
	}

	::GetClientRect(mhWnd, &rect);	

	mWidth = rect.right - rect.left;
	mHeight = rect.bottom - rect.top;
}

void Window::ForceCursorToCenter()
{
	POINT pt;
	pt.x = mWidth >> 2;
	pt.y = mHeight >> 2;

	::ClientToScreen(mhWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	ShowCursor(false);
}

} // Namespace RcEngine