#ifndef D3D11RenderState_h__
#define D3D11RenderState_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHState.h>

namespace RcEngine {

class _D3D11Export D3D11DepthStencilState : public RHDepthStencilState
{
public:
	D3D11DepthStencilState(const RHDepthStencilStateDesc& desc);
	~D3D11DepthStencilState();

public:
	ID3D11DepthStencilState* StateD3D11;
};

class _D3D11Export D3D11BlendState : public RHBlendState
{
public:
	D3D11BlendState(const RHBlendStateDesc& desc);
	~D3D11BlendState();

public:
	ID3D11BlendState* StateD3D11;
};

class _D3D11Export D3D11RasterizerState : public RHRasterizerState
{
public:
	D3D11RasterizerState(const RHRasterizerStateDesc& desc);
	~D3D11RasterizerState();

public:
	ID3D11RasterizerState* StateD3D11;
};

class _D3D11Export D3D11SamplerState : public RHSamplerState
{
public:
	D3D11SamplerState(const RHSamplerStateDesc& desc);
	~D3D11SamplerState();

public:
	ID3D11SamplerState* StateD3D11;
};
	
}



#endif // D3D11RenderState_h__