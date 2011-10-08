#include "WindowEventUtils.h"
#include "RenderWindow.h"


namespace Engine
{

	
	WindowEventUtilities::WindowEventListeners WindowEventUtilities::msListeners;
	WindowEventUtilities::Windows WindowEventUtilities::msWindows;

	LRESULT CALLBACK WindowEventUtilities::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
#pragma warning (push)
#pragma warning (disable : 4244)
#pragma warning (disable : 4312)
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		RenderWindow* win = reinterpret_cast<RenderWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

#pragma warning (pop)

		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		//Iterator of all listeners registered to this RenderWindow
		WindowEventListeners::iterator index,
			start = msListeners.begin(),
			end = msListeners.end();

		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				bool active = (LOWORD(wParam) != WA_INACTIVE);
				if( active )
				{
					win->SetActive( true );
				}
				else
				{
					if( win->IsDeactivatedOnFocusChange() )
					{
						win->SetActive( false );
					}
				}

				for( ; start != end; ++start )
					 (start->second)->WindowFocusChange(win);
				break;
			}
		case WM_SYSKEYDOWN:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSKEYUP:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
			case VK_F10:
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSCHAR:
			// return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
			if (wParam != VK_SPACE)
				return 0;
			break;
		case WM_ENTERSIZEMOVE:
			
			break;
		case WM_EXITSIZEMOVE:
			
			break;
		case WM_MOVE:
			win->WindowMovedOrResized();
			for(index = start; index != end; ++index)
				  (index->second)->WindowMoved(win);
			break;
		case WM_DISPLAYCHANGE:
			win->WindowMovedOrResized();
			for(index = start; index != end; ++index)
				(index->second)->WindowResized(win);
			break;
		case WM_SIZE:
			//log->logMessage("WM_SIZE");
			win->WindowMovedOrResized();
			for(index = start; index != end; ++index)
				(index->second)->WindowResized(win);
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:
			{
				bool close = true;
				for(index = start; index != end; ++index)
				{
					if (!(index->second)->WindowClosing(win))
						close = false;
				}
				if (!close) return 0;

				for(index = msListeners.begin(); index != end; ++index)
					(index->second)->WindowClosed(win);
				win->Destroy();
				return 0;
			}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}

		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	void WindowEventUtilities::AddWindowEventListener( RenderWindow* window, WindowEventListener* listener )
	{
		msListeners.insert(std::make_pair(window, listener));
	}

	void WindowEventUtilities::RemoveWindowEventListener( RenderWindow* window, WindowEventListener* listener )
	{
		WindowEventListeners::iterator i = msListeners.begin(), e = msListeners.end();

		for( ; i != e; ++i )
		{
			if( i->first == window && i->second == listener )
			{
				msListeners.erase(i);
				break;
			}
		}
	}

	void WindowEventUtilities::AddRenderWindow( RenderWindow* window )
	{
		msWindows.push_back(window);
	}

	void WindowEventUtilities::RemoveRenderWindow( RenderWindow* window )
	{
		Windows::iterator i = std::find(msWindows.begin(), msWindows.end(), window);
		if( i != msWindows.end() )
			msWindows.erase( i );
	}
}