#ifndef _RenderDevice__H
#define _RenderDevice__H

#include "Common.h"

namespace GlueRH
{
	class RenderDevice
	{
	public:
		RenderDevice(void);
		~RenderDevice(void);

		void InitD3D(int width, int height);
		void DestroyAll();

	protected:
		ID3D10Device* mDevice;				// The IDirect3DDevice10 interface
		ID3D10Texture2D* mBackBuffer;			// Pointer to the back buffer
		ID3D10Texture2D* mDepthStencilBuffer;  // Pointer to the depth/stencil buffer
		ID3D10DepthStencilState* mDepthStencilState;   // The depth/stencil buffer state
		ID3D10DepthStencilView* mDepthStencilView;    // View of the depth stencil buffer
		ID3D10RenderTargetView* mRenderTargetView;    // Pointer to the render target view
		IDXGISwapChain* mSwapChain;			// Pointer to the swap chain

		RECT	m_rcScreenRect;					// The dimensions of the screen


		ID3DX10Font*			mFont;		// The font used for rendering text
		ID3DX10Sprite*			mFontSprite;	// Sprites used to hold font characters

	};
}


#endif
