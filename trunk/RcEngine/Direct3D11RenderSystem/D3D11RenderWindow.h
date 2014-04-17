#ifndef D3D11RenderWindow_h__
#define D3D11RenderWindow_h__
#pragma once

#include "D3D11Prerequisites.h"
#include "D3D11FrameBuffer.h"

namespace RcEngine {

struct  RenderSettings;

class _D3D11Export D3D11RenderWindow : public D3D11FrameBuffer
{
public:
	D3D11RenderWindow();
	~D3D11RenderWindow();

	void ToggleFullscreen(bool fs);
	void SwapBuffers();
public:
	IDXGISwapChain* SwapChainD3D11;
	D3D_FEATURE_LEVEL FeatureLevelD3D11;

private:
	std::string mDescription;
	uint32_t mSyncInterval;
	HWND mHwnd;
};

}


#endif // D3D11RenderWindow_h__
