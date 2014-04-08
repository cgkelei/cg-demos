#include "D3D11RenderState.h"

namespace RcEngine {

ID3D11Device* g_pd3dDevice;

D3D11DepthStencilState::D3D11DepthStencilState( const DepthStencilStateDesc& desc )
	:DepthStencilState(desc)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = mDesc.DepthEnable;
	depthStencilDesc.StencilEnable = mDesc.StencilEnable;
	depthStencilDesc.DepthWriteMask = mDesc.DepthWriteMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.StencilReadMask = mDesc.StencilReadMask;
	depthStencilDesc.StencilWriteMask = mDesc.DepthWriteMask;
	depthStencilDesc.DepthFunc = D


}

D3D11DepthStencilState::~D3D11DepthStencilState()
{

}


D3D11BlendState::D3D11BlendState( const BlendStateDesc& desc )
{

}

D3D11BlendState::~D3D11BlendState()
{

}


D3D11RasterizerState::D3D11RasterizerState( const RasterizerStateDesc& desc )
{

}

D3D11RasterizerState::~D3D11RasterizerState()
{

}


D3D11SamplerState::D3D11SamplerState( const SamplerStateDesc& desc )
{

}

D3D11SamplerState::~D3D11SamplerState()
{

}

}
