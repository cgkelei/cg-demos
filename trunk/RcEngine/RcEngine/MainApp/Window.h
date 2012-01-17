#ifndef Window_h__
#define Window_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderSettings.h>

namespace RcEngine {

class _ApiExport Window
{
public:
	Window(const std::string& name, const RenderSystem::RenderSettings& settings);
	~Window(void);

	HWND GetHwnd() const		{ return mhWnd; }
	
	void Reposition(int left, int top);

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
	unsigned int mLeft, mTop;
	unsigned int mWidth, mHeight;
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