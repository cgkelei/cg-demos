#include "D3D11RenderDevice.h"
#include "D3D11RenderState.h"
#include <Core/Exception.h>

namespace RcEngine {

void D3D11RenderDevice::Create()
{

}

void D3D11RenderDevice::Release()
{

}

void D3D11RenderDevice::ToggleFullscreen( bool fs )
{

}

bool D3D11RenderDevice::Fullscreen() const
{

}

void D3D11RenderDevice::CreateRenderWindow( const RenderSettings& settings )
{

}

void D3D11RenderDevice::AdjustProjectionMatrix( float4x4& pOut )
{

}

void D3D11RenderDevice::DoBindFrameBuffer( const shared_ptr<FrameBuffer>& fb )
{

}

void D3D11RenderDevice::DoRender( EffectTechnique& tech, RenderOperation& op )
{

}

void D3D11RenderDevice::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state )
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
		ID3D11SamplerState* samplerStateD3D11 = (static_pointer_cast_checked<D3D11SamplerState>(state))->StateD3D11;
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

void D3D11RenderDevice::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
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

void D3D11RenderDevice::SetRasterizerState( const shared_ptr<RasterizerState>& state )
{
	if (mCurrentRasterizerState != state)
	{
		mDeviceContext->RSSetState((static_pointer_cast_checked<D3D11RasterizerState>(state))->StateD3D11);
		mCurrentRasterizerState = state;
	}
}

void D3D11RenderDevice::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef /*= 0*/, uint16_t backStencilRef /*= 0*/ )
{
	if (mCurrentDepthStencilState != state || frontStencilRef != mCurrentFrontStencilRef || mCurrentBackStencilRef != mCurrentBackStencilRef)
	{
		mDeviceContext->OMSetDepthStencilState(
			(static_pointer_cast_checked<D3D11DepthStencilState>(state))->StateD3D11, 
			frontStencilRef);

		mCurrentDepthStencilState = state;
		mCurrentFrontStencilRef = frontStencilRef;
		mCurrentBackStencilRef = backStencilRef;
	}
}

}