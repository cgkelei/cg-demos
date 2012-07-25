#ifndef Window_h__
#define Window_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderSettings.h>

namespace RcEngine {

class _ApiExport Window
{
public:
	Window(const std::string& name, const Render::RenderSettings& settings);
	~Window(void);

	HWND GetHwnd() const		{ return mhWnd; }
	uint32_t GetWidth() const	{ return mWidth; }
	uint32_t GetHeight() const  { return mHeight; }
	uint32_t GetLeft() const	{ return mLeft; }
	uint32_t GetTop() const		{ return mTop; }
	
	void Reposition(int32_t left, int32_t top);
	void ShowWindow();

	bool InSizeMoveTest() const { return mInSizeMove; }

private:
	void OnUserResized();
	void OnSuspend();
	void OnResume();
	void OnApplicationActivated();
	void OnApplicationDeactivated();
	void OnPaint();

public:
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	EventHandler PaintEvent;
	EventHandler ResumeEvent;
	EventHandler UserResizedEvent;
	EventHandler SuspendEvent;
	EventHandler ApplicationActivatedEvent;
	EventHandler ApplicationDeactivatedEvent;

private:
	uint32_t mLeft, mTop;
	uint32_t mWidth, mHeight;
	std::string mName;

	bool mFullscreen;
	
	bool mMinimized, mMaximized;
	bool mInSizeMove;

	HWND mhWnd;
	HINSTANCE mhInstance;
public:
	static Window* msWindow;

};

} // Namespace RcEngine

#endif // Window_h__