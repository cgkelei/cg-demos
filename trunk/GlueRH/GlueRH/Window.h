#ifndef _Window__H
#define _Window__H

#include "Common.h"
#include <utility>

namespace GlueRH
{
	class Window
	{ 
	public:
		typedef std::pair<int, int> Size;

		explicit Window( const std::wstring& name, int left, int top, int32 width, int32 height, bool fullscreen = false);
		~Window(void);

		EventHandler& EventUserResized() { return UserResized; }
		EventHandler& EventSuspend() { return Suspend; }
		EventHandler& EventResume() { return Resume; }
		EventHandler& EventPaint() { return Paint; }
		EventHandler& EventApplicationActivated() { return ApplicationActivated; }
		EventHandler& EventApplicationDeactivated() { return ApplicationDeactivated; }
		EventHandler& EventSystemSuspend() { return SystemSuspend; }
		EventHandler& EventSystemResume() { return SystemResume; }
		EventHandler& EventMonitorChanged() { return MonitorChanged; }
		CancellableEventHandler& EventScreensaver() { return Screensaver; }

		HWND GetWindowHandle() const { return mHwnd; } 
		bool IsMinimized() const { return mIsMinimized; }
		bool IsMaximized() const { return mIsMaximized; }
		std::wstring GetWindowTitle() const  { return mName; }
		Size GetCurrentSize() const;

		void LoadWindow();
		void CloseWindow();

		static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	private:
		void MyRegisterClass();
		bool InitInstance();
		void UpdateMonitor();

	protected:

		virtual void OnUserResized();
		virtual void OnSuspend();
		virtual void OnMonitorChanged();
		virtual void OnApplicationActivated();
		virtual void OnApplicationDeactivated();
		virtual void OnSystemSuspend();
		virtual void OnSystemResume();
		virtual void OnScreensaver(bool* cancel);
		virtual void OnResume();
		virtual void OnPaint();

	

	private:
		/// <summary>
		/// Occurs when the application is suspended.
		/// </summary>
		EventHandler Suspend;

		/// <summary>
		/// Occurs when the application is resumed.
		/// </summary>
		EventHandler Resume;

		/// <summary>
		/// Occurs when the user resizes the window.
		/// </summary>
		EventHandler UserResized;

		/// <summary>
		/// Occurs when the window is draw.
		/// </summary>
		EventHandler Paint;

		/// <summary>
		/// Occurs when the screen on which the window resides is changed.
		/// </summary>
		EventHandler MonitorChanged;

		/// <summary>
		/// Occurs when the application is activated.
		/// </summary>
		EventHandler ApplicationActivated;

		/// <summary>
		/// Occurs when the application is deactivated.
		/// </summary>
		EventHandler ApplicationDeactivated;

		/// <summary>
		/// Occurs when the system is suspended.
		/// </summary>
		EventHandler SystemSuspend;

		/// <summary>
		/// Occurs when the system is resumed.
		/// </summary>
		EventHandler SystemResume;

		/// <summary>
		/// Occurs when a screen saver is about to be activated.
		/// </summary>
		CancellableEventHandler Screensaver;


	protected:

		int32 mLeft;
		int32 mTop;
		int32 mWidth;
		int32 mHeight;
		
		bool mFullscreen;
		bool mActive;

		bool mIsMinimized;
		bool mIsMaximized;
		bool mIsInSizeMove;

		Size mCachedSize;

		int32 mMouseWheel;
			
		std::wstring mName;

		HWND mHwnd;
		HINSTANCE mHinst;
		HMONITOR mCurrentMonitor;

		static Window* m_pGlobalWindow;


	};

}

#endif