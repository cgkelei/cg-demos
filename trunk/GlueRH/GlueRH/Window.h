#ifndef _Window__H
#define _Window__H

#include "Common.h"

namespace GlueRH
{
	class Window
	{ 
	public:
		explicit Window( const std::wstring& name, int left, int top, int32 width, int32 height, bool fullscreen = false);
		~Window(void);

		void InitWindow();
		
		ActiveEvent& OnActive() { return mActiveEventHandler; }
		PaintEvent& OnPaint() { return mPaintEventHandler; }
		EnterSizeMoveEvent& OnEnterSizeMove() { return mEnterSizeMoveEventHandler; }
		ExitSizeMoveEvent& OnExitSizeMove() { return mExitSizeMoveEventHandler; }
		SizeEvent& OnSize() { return mSizeEventHandler; }
		SetCursorEvent& OnSetCursor() { return mSetCursorEventHandler; }
		CloseEvent& OnClose() { return mCloseEventHandler; }

		HWND GetWindowHandle() const { return mHwnd; } 
		std::wstring GetWindowTitle() const  { return mName; }

		static Window* GetMainWindow() { return m_pGlobalWindow; }
		
		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	private:
		void MyRegisterClass();
		bool InitInstance();

	private:
		// Windows Event Handler
		ActiveEvent mActiveEventHandler;
		PaintEvent mPaintEventHandler;
		EnterSizeMoveEvent mEnterSizeMoveEventHandler;
		ExitSizeMoveEvent mExitSizeMoveEventHandler;
		SizeEvent mSizeEventHandler;
		SetCursorEvent mSetCursorEventHandler;
		CloseEvent mCloseEventHandler;

	protected:

		int32 mLeft;
		int32 mTop;
		int32 mWidth;
		int32 mHeight;
		
		bool mFullscreen;
		bool mActive;

		std::wstring mName;

		HWND mHwnd;

		static Window* m_pGlobalWindow;


	};

	inline Window* MainWindow() 
	{
		return Window::GetMainWindow();
	}

}

#endif