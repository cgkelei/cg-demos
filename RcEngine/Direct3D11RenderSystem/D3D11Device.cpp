#include "D3D11Device.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "D3D11Factory.h"
#include "D3D11RenderWindow.h"
#include "D3D11VertexDeclaration.h"
#include "D3D11GraphicCommon.h"
#include <Graphics/RenderOperation.h>
#include <Graphics/Effect.h>
#include <MainApp/Application.h>
#include <Core/Exception.h>

namespace RcEngine {

D3D11Device* gD3D11Device = nullptr;

D3D11Device::D3D11Device( )
	: DeviceContextD3D11(nullptr),
	  DeviceD3D11(nullptr)
{
	gD3D11Device = this;
	mRHFactory = new D3D11Factory();
}

D3D11Device::~D3D11Device()
{
	SAFE_RELEASE(DeviceD3D11);
	SAFE_RELEASE(DeviceContextD3D11);
}

void D3D11Device::SetSamplerState( ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state )
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

void D3D11Device::SetBlendState( const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask )
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

void D3D11Device::SetRasterizerState( const shared_ptr<RasterizerState>& state )
{
	if (mCurrentRasterizerState != state)
	{
		DeviceContextD3D11->RSSetState((static_cast<D3D11RasterizerState*>(state.get()))->StateD3D11);
		mCurrentRasterizerState = state;
	}
}

void D3D11Device::SetDepthStencilState( const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef /*= 0*/, uint16_t backStencilRef /*= 0*/ )
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

void D3D11Device::SetViewport( const Viewport& vp )
{
	CD3D11_VIEWPORT vpD3D11(float(vp.Left), float(vp.Top), float(vp.Width), float(vp.Height));
	DeviceContextD3D11->RSSetViewports(1, &vpD3D11);
}

void D3D11Device::CreateRenderWindow()
{
	const ApplicationSettings& appSettings = Application::msApp->GetAppSettings();

	shared_ptr<D3D11RenderWindow> d3d11RenderWindow = std::make_shared<D3D11RenderWindow>(appSettings.Width, appSettings.Height);
	
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
		RenderFactory* factory = gD3D11Device->GetRenderFactory();
		shared_ptr<Texture> depthStencilTexture = factory->CreateTexture2D(
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

		d3d11RenderWindow->AttachRTV(ATT_DepthStencil, factory->CreateDepthStencilView(depthStencilTexture, 0, 0));
	}

	d3d11RenderWindow->SetViewport(Viewport(0, 0, appSettings.Width, appSettings.Height));
	BindFrameBuffer(d3d11RenderWindow);

	mScreenFrameBuffer = d3d11RenderWindow;
}

void D3D11Device::ToggleFullscreen( bool fs )
{

}

void D3D11Device::AdjustProjectionMatrix( float4x4& pOut )
{

}

void D3D11Device::DoDraw( const EffectTechnique* technique, const RenderOperation& operation )
{
	// Set up input layout
	D3D11VertexDeclaration* vertexDeclD3D11 = static_cast_checked<D3D11VertexDeclaration*>(operation.VertexDecl.get());
	if (vertexDeclD3D11->InputLayoutD3D11 == nullptr)
	{
		const Shader& vertexShader = *(technique->GetPassByIndex(0)->GetShaderPipeline()->GetShader(ST_Vertex));
		vertexDeclD3D11->CreateInputLayout(operation, vertexShader);
	}
	DeviceContextD3D11->IASetInputLayout(vertexDeclD3D11->InputLayoutD3D11);

	// Set up vertex stream
	if (operation.VertexStreams.size())
	{
		static ID3D11Buffer* vertexStreamsD3D11[8];
		static uint32_t streamStride[8];
		static uint32_t streamOffset[8] = {0};

		for (size_t i = 0; i < operation.VertexStreams.size(); ++i)
		{
			vertexStreamsD3D11[i] = static_cast_checked<D3D11Buffer*>(operation.VertexStreams[i].get())->BufferD3D11;
			streamStride[i] = vertexDeclD3D11->GetStreamStride(i);
		}

		DeviceContextD3D11->IASetVertexBuffers(0, operation.VertexStreams.size(), vertexStreamsD3D11, streamStride, streamOffset);
	}

	// Set draw primitive mode
	DeviceContextD3D11->IASetPrimitiveTopology( D3D11Mapping::Mapping(operation.PrimitiveType) );
	if (operation.IndexBuffer)
	{
		ID3D11Buffer* indexBufferD3D11 = static_cast_checked<D3D11Buffer*>(operation.IndexBuffer.get())->BufferD3D11;

		/**
		 * StartIndexLocation and Index Buffer Offset is almost the same thing. 
		 * The difference is that the offset is in bytes so you have to take the 
		 * index size (16 bit vs 32 bit) into account. 
		 */

		//uint32_t indexOffset;
		//DXGI_FORMAT indexFormatD3D11;
		//if(operation.IndexType == IBT_Bit16)
		//{
		//	indexFormatD3D11 = DXGI_FORMAT_R16_UINT;
		//	indexOffset += operation.IndexStart * 2;
		//}
		//else
		//{
		//	indexFormatD3D11 = DXGI_FORMAT_R32_UINT;
		//	indexOffset += operation.IndexStart * 4;
		//}
		//DeviceContextD3D11->IASetIndexBuffer(indexBufferD3D11, indexFormatD3D11, indexOffset);

		DXGI_FORMAT indexFormatD3D11 = (operation.IndexType == IBT_Bit16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		DeviceContextD3D11->IASetIndexBuffer(indexBufferD3D11, indexFormatD3D11, 0);

		for (EffectPass* pass : technique->GetPasses())
		{
			pass->BeginPass();

			if (operation.NumInstances <= 1)
			{
				DeviceContextD3D11->DrawIndexed(
					operation.IndexCount, 
					operation.IndexStart,
					0 /*operation.BaseVertex*/);
			}
			else
			{
				DeviceContextD3D11->DrawIndexedInstanced(
					operation.IndexCount, 
					operation.NumInstances,
					operation.IndexStart,
					0 /*operation.BaseVertex*/,
					0);
			}

			pass->EndPass();
		}
	}
	else
	{
		for (EffectPass* pass : technique->GetPasses())
		{
			pass->BeginPass();

			D3D11_VIEWPORT viewport;
			UINT numViewports = 1;
			DeviceContextD3D11->RSGetViewports(&numViewports, &viewport);

			D3D11_PRIMITIVE_TOPOLOGY priType;
			DeviceContextD3D11->IAGetPrimitiveTopology(&priType);

			ID3D11ShaderResourceView* srv;
			DeviceContextD3D11->PSGetShaderResources(0, 1, &srv);

			ID3D11Buffer *vb, *ib;
			uint32_t stride, offset;
			DXGI_FORMAT indexFormat;
			DeviceContextD3D11->IAGetVertexBuffers(0, 1, &vb, &stride, &offset);
			DeviceContextD3D11->IAGetIndexBuffer(&ib, &indexFormat, &offset);

			ID3D11InputLayout* layout;
			DeviceContextD3D11->IAGetInputLayout(&layout);

			if (operation.NumInstances <= 1)
			{
				DeviceContextD3D11->Draw(
					operation.VertexCount,
					operation.VertexStart);
			}
			else
			{
				DeviceContextD3D11->DrawInstanced(
					operation.VertexCount,
					operation.NumInstances,
					operation.VertexStart,
					0);
			}

			pass->EndPass();
		}
	}
}

}