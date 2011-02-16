#pragma once

#include "Config.h"
#include "Vector4.h"
#include "Viewport.h"
#include "ColorRGBA.h"
class Camera;


class Renderer
{
public:
	Renderer(HWND hwnd, int32 width, int height);
	~Renderer(void);

	Camera* GetCamera()		{ return m_camera; }
	void    SetCamera(Camera* camera) { m_camera = camera; }	
	virtual void beginFrame();
	virtual void drawFrame();
	virtual void endFrame();
	virtual void OnSize(const int width, const int height);
	virtual void InitialOpenGL();
	virtual void initDevice();
	virtual void ClearBackBuffer();
	virtual void ClearDepthBuffer();
	virtual void ClearStencilBuffer();
	virtual void SwapBuffer();	 //swap back buffer, display
	virtual void SetViewport(int X, int Y, int width, int height);


	const Viewport& GetViewport() const  {  return m_viewport; }

protected:
	HWND		m_hwnd;
	HDC			m_hdc;
	HGLRC		m_hrc;

	int32		m_width;
	int32		m_height;
	ColorRGBA	m_clearColor;
	float		m_clearDepth;
	float		m_clearStencil;

	Viewport	m_viewport;
	Camera*		m_camera;

};
