#ifndef D3D11RenderView_h__
#define D3D11RenderView_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RenderView.h>

namespace RcEngine {

class D3D11Texture2D;

class _D3D11Export D3D11RenderView : public RenderView
{
public:
	D3D11RenderView(void);
	~D3D11RenderView(void);
};


class _D3D11Export D3D11DepthStencilView : public D3D11RenderView
{
public:
	D3D11DepthStencilView(Texture& texture, uint32_t arrIndex, uint32_t level);
	~D3D11DepthStencilView();


private:
	uint32_t mArrIndex;
	uint32_t mLevel; 
	D3D11Texture2D& mTextureD3D11;
	ID3D11DepthStencilView* mDepthStencilView;
};


class _D3D11Export D3D11RenderTargetView2D : public D3D11RenderView
{
public:
	D3D11RenderTargetView2D(Texture& texture, uint32_t arrIndex, uint32_t level);
	~D3D11RenderTargetView2D();

private:
	uint32_t mArrIndex;
	uint32_t mLevel; 
	D3D11Texture2D& mTextureD3D11;
	ID3D11RenderTargetView* mRenderTargetView2D;
};

//class _D3D11Export D3D11RenderTargetView3D : public D3D11RenderView
//{
//public:
//	D3D11RenderTargetView3D(Texture& texture, )
//
//private:
//	D3D11Texture3D& mTextureD3D11;
//	ID3D11RenderTargetView* mRenderTargetView3D;
//};

//class _D3D11Export D3D11RenderTargetViewCube : public D3D11RenderView
//{
//public:
//
//
//private:
//	D3D11TextureCube& mTextureD3D11;
//	ID3D11RenderTargetView* mRenderTargetViewCube;
//};

}

#endif // D3D11RenderView_h__

