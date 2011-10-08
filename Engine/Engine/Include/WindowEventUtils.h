#pragma once
#ifndef _WindowEventUtils__H
#define _WindowEventUtils__H

#include "Prerequisites.h"
#include "Listeners.h"

namespace Engine
{

	class _ApiExport WindowEventUtilities
	{
	public:

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		
		/// <remarks>
		/// Add a listener to listen to renderwindow events (multiple listener's per renderwindow is fine)
		/// The same listener can listen to multiple windows, as the Window Pointer is sent along with
		/// any messages.
		/// </remarks>
		/// <param name = "window"> The RenderWindow you are interested in monitoring </param>
		/// <param name =" listner"> Your callback listener. </param>
		static void AddWindowEventListener( RenderWindow* window, WindowEventListener* listener );

		/// <summary>
		/// Remove previously added listener
		/// </summary>
		static void RemoveWindowEventListener( RenderWindow* window, WindowEventListener* listener );

	
		static void AddRenderWindow(RenderWindow* window);


		static void RemoveRenderWindow(RenderWindow* window);

	public:

#pragma warning (push)
#pragma warning (disable : 4251)
		
		/*typedef std::vector<WindowEventListener*> WindowEventListeners;
		static WindowEventListeners msListeners;
		static RenderWindow* msWindows;*/

		typedef std::multimap<RenderWindow*, WindowEventListener*> WindowEventListeners;
		static WindowEventListeners msListeners;

		typedef std::vector<RenderWindow*> Windows;
		static Windows msWindows;


#pragma warning (pop)

	};
}


#endif // _WindowEventUtils__H