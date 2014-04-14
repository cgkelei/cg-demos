#include "D3D11Device.h"
#include "D3D11State.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Device* gD3D11Device = nullptr;

D3D11Device::D3D11Device( const RenderSettings& settings )
	: RHDevice(settings),
	  mDeviceContext(nullptr),
	  mDevice(nullptr)
{

}


void D3D11Device::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<RHSamplerState>& state )
{
	SamplerSlot samSlot(stage, unit);

	bool needUpdate = false;
	if (mCurrentSamplers.find(samSlot) != mCurrentSamplers.end())
	{
		if (mCurrentSamplers[samSlot] != state)
		{
			needUpdate = true;
			mCurrentSamplers[samSlot] = state;
		}
	}
	else
	{
		needUpdate = true;
		mCurrentSamplers[samSlot] = state;
	}

	if (needUpdate)
	{
		ID3D11SamplerState* samplerStateD3D11 = (static_cast<D3D11SamplerState*>(state.get()))->StateD3D11;
		switch (stage)
		{
		case ST_Vertex:
			mDeviceContext->VSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Geomerty:
			mDeviceContext->GSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Pixel:
			mDeviceContext->PSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Compute:
			mDeviceContext->CSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		default:
			ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid SamplerState", "D3D11RenderDevice::SetSamplerState");
			break;
		}
	}
}

void D3D11Device::SetBlendState( const shared_ptr<RHBlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
{
	if (mCurrentBlendState != state || mCurrentBlendFactor != blendFactor || mCurrentSampleMask != sampleMask)
	{
		mDeviceContext->OMSetBlendState( 
			(static_pointer_cast_checked<D3D11BlendState>(state))->StateD3D11,			
			blendFactor(), 
			sampleMask);

		mCurrentBlendState = state;
		mCurrentBlendFactor = blendFactor;
		mCurrentSampleMask = sampleMask;
	}
}

void D3D11Device::SetRasterizerState( const shared_ptr<RHRasterizerState>& state )
{
	if (mCurrentRasterizerState != state)
	{
		mDeviceContext->RSSetState((static_cast<D3D11RasterizerState*>(state.get()))->StateD3D11);
		mCurrentRasterizerState = state;
	}
}

void D3D11Device::SetDepthStencilState( const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef /*= 0*/, uint16_t backStencilRef /*= 0*/ )
{
	if (mCurrentDepthStencilState != state || frontStencilRef != mCurrentFrontStencilRef || mCurrentBackStencilRef != mCurrentBackStencilRef)
	{
		mDeviceContext->OMSetDepthStencilState(
			(static_cast<D3D11DepthStencilState*>(state.get()))->StateD3D11,
			frontStencilRef);

		mCurrentDepthStencilState = state;
		mCurrentFrontStencilRef = frontStencilRef;
		mCurrentBackStencilRef = backStencilRef;
	}
}

}