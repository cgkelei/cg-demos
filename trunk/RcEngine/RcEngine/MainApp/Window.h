#ifndef Window_h__
#define Window_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderSettings.h>

namespace RcEngine {

class _ApiExport Window
{
public:
	Window(const std::string& name, const RenderSettings& settings);
	~Window(void);

	HWND GetHwnd() const		{ return mhWnd; }
	uint32_t GetWidth() const	{ return mWidth; }
	uint32_t GetHeight() const  { return mHeight; }
	uint32_t GetLeft() const	{ return mLeft; }
	uint32_t GetTop() const		{ return mTop; }
	
	bool InSizeMove() const		{ return mInSizeMove; }
	bool IsMouseVisible() const { return mMouseVisible; }

	void Reposition(int32_t left, int32_t top);
	void ShowWindow();
	void SetTitle(const String& title);
	void ForceCursorToCenter();
	void SetMouseVisible(bool visible);

	// After Resize or reposition, this will compute new window size
	void UpdateWindowSize();

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

	bool mMouseVisible;
	bool mFullscreen;
	bool mInSizeMove;
	bool mMinimized, mMaximized;


	HWND mhWnd;
	HINSTANCE mhInstance;
public:
	static Window* msWindow;

};

} // Namespace RcEngine

#endif // Window_h__