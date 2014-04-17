#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11RenderWindow.h"
#include "D3D11Factory.h"
#include <MainApp/Application.h>
#include <Core/Exception.h>

namespace RcEngine {

D3D11Device* gD3D11Device = nullptr;

D3D11Device::D3D11Device( )
	: DeviceContextD3D11(nullptr),
	  DeviceD3D11(nullptr)
{

}

D3D11Device::~D3D11Device()
{
	SAFE_RELEASE(DeviceD3D11);
	SAFE_RELEASE(DeviceContextD3D11);
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
			DeviceContextD3D11->VSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Geomerty:
			DeviceContextD3D11->GSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Pixel:
			DeviceContextD3D11->PSSetSamplers(unit, 1, &samplerStateD3D11);
			break;
		case ST_Compute:
			DeviceContextD3D11->CSSetSamplers(unit, 1, &samplerStateD3D11);
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
		DeviceContextD3D11->OMSetBlendState( 
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
		DeviceContextD3D11->RSSetState((static_cast<D3D11RasterizerState*>(state.get()))->StateD3D11);
		mCurrentRasterizerState = state;
	}
}

void D3D11Device::SetDepthStencilState( const shared_ptr<RHDepthStencilState>& state, uint16_t frontStencilRef /*= 0*/, uint16_t backStencilRef /*= 0*/ )
{
	if (mCurrentDepthStencilState != state || frontStencilRef != mCurrentFrontStencilRef || mCurrentBackStencilRef != mCurrentBackStencilRef)
	{
		DeviceContextD3D11->OMSetDepthStencilState(
			(static_cast<D3D11DepthStencilState*>(state.get()))->StateD3D11,
			frontStencilRef);

		mCurrentDepthStencilState = state;
		mCurrentFrontStencilRef = frontStencilRef;
		mCurrentBackStencilRef = backStencilRef;
	}
}

void D3D11Device::SetViewport( const RHViewport& vp )
{
	CD3D11_VIEWPORT vpD3D11(float(vp.Left), float(vp.Top), float(vp.Width), float(vp.Height));
	DeviceContextD3D11->RSSetViewports(1, &vpD3D11);
}

void D3D11Device::DoDraw( const RHOperation& operation )
{

}

void D3D11Device::CreateRenderWindow()
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	shared_ptr<D3D11RenderWindow> d3d11RenderWindow = std::make_shared<D3D11RenderWindow>();
	
	HRESULT hr = S_OK;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = d3d11RenderWindow->SwapChainD3D11->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
	{
		// ERROR
	}

	ID3D11RenderTargetView* pRenderTargetView = NULL;
	hr = gD3D11Device->DeviceD3D11->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) )
	{
		// ERROR
	}
	d3d11RenderWindow->AttachRTV(ATT_Color0, std::make_shared<D3D11RenderTargetView2D>(pRenderTargetView) );

	if(PixelFormatUtils::IsDepth(appSettings.DepthStencilFormat))
	{
		// Have depth buffer, attach it
		RHFactory* factory = gD3D11Device->GetFactory();
		shared_ptr<RHTexture> depthStencilTexture = factory->CreateTexture2D(
			appSettings.Width, 
			appSettings.Height,
			appSettings.DepthStencilFormat, 
			1, 
			1, 
			appSettings.SampleCount, 
			appSettings.SampleQuality,
			EAH_GPU_Read | EAH_GPU_Write,
			TexCreate_DepthStencilTarget,
			nullptr);

		shared_ptr<RHRenderView> depthStencilView = factory->CreateDepthStencilView(depthStencilTexture, 0, 0);

		d3d11RenderWindow->AttachRTV(ATT_DepthStencil, depthStencilView);
	}

	BindFrameBuffer(d3d11RenderWindow);
	mScreenFrameBuffer = d3d11RenderWindow;
}

}