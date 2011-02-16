#pragma once
#include "Config.h"

class Renderer;
class KeyboardHandler;
class MouseHandler;
class Camera;

class Application
{
public:
	Application(const std::string& title, int32 width, int32 height);
	virtual ~Application(void);
	
	static Application* Instance()  { return ms_app; }

public:
	int32				GetWidth() { return m_width;}
	int32				GetHeight() { return m_height;}
	MouseHandler*		GetMouseHandler() const  { return m_mouseHandler; }
	KeyboardHandler*	GetKeyboardHandler() const  { return m_keyboardHandler; }
	Renderer*			GetRenderer() const { return m_renderer; }

public:

public:
	void OnSize(const int width, const int height);
	void Run();	

private: 
	static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void CreateRenderer(HWND hwnd, int32 width, int height);

protected: 
	void CreateWin();

protected:
	int32	m_width;
	int32	m_height;
	std::string	m_title;
	
	bool		m_active;
	HWND		m_hwnd;
	HINSTANCE	m_hInst;

	KeyboardHandler*		m_keyboardHandler;
	MouseHandler*			m_mouseHandler;
	Renderer*				m_renderer;
	Camera*					m_camera;
	static Application*		ms_app;

};

