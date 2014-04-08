#ifndef D3D11RenderState_h__
#define D3D11RenderState_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/DepthStencilState.h>
#include <Graphics/BlendState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/SamplerState.h>

namespace RcEngine {

class _D3D11Export D3D11DepthStencilState : public DepthStencilState
{
public:
	D3D11DepthStencilState(const DepthStencilStateDesc& desc);
	~D3D11DepthStencilState();

private:
	ID3D11DepthStencilState* IDepthStencilState;
};

class _D3D11Export D3D11BlendState : public BlendState
{
public:
	D3D11BlendState(const BlendStateDesc& desc);
	~D3D11BlendState();

private:
	ID3D11BlendState* mBlendState;

};

class _D3D11Export D3D11RasterizerState : public RasterizerState
{
public:
	D3D11RasterizerState(const RasterizerStateDesc& desc);
	~D3D11RasterizerState();

private:
	ID3D11RasterizerState* mRasterizerState;
};

class _D3D11Export D3D11SamplerState : public SamplerState
{
public:
	D3D11SamplerState(const SamplerStateDesc& desc);
	~D3D11SamplerState();


private:
	ID3D11SamplerState* mSamplerState;
}
	
	


}



#endif // D3D11RenderState_h__