#include "D3D11FrameBuffer.h"
#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"
#include <Core/Exception.h>

namespace RcEngine {

	D3D11DepthStencilView::D3D11DepthStencilView( ID3D11DepthStencilView* dsv )
	: RHRenderView(nullptr),
	  DepthStencilViewD3D11(dsv)
{

}

D3D11DepthStencilView::D3D11DepthStencilView( const shared_ptr<RHTexture>& texture, uint32_t arrIndex, uint32_t level )
	: RHRenderView(texture),
	  DepthStencilViewD3D11(nullptr)
{
	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_DepthStencilTarget);

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	uint32_t textureArraySize = texture->GetTextureArraySize();
	if (textureArraySize <= 1)
	{
		if (texture->GetSampleCount() > 1)
		{
			viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			viewDesc.Texture2D.MipSlice = level;
			viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}
	}
	else
	{
		if (texture->GetSampleCount() > 1)
		{
			viewDesc.Texture2DMSArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DMSArray.ArraySize = 1;
			viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		}
		else
		{
			viewDesc.Texture2DArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DArray.ArraySize = 1;
			viewDesc.Texture2DArray.MipSlice = level;
			viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		}
	}

	ID3D11Texture2D* textureD3D11 = (static_cast<D3D11Texture2D*>(texture.get()))->TextureD3D11;
	HRESULT hr = gD3D11Device->DeviceD3D11->CreateDepthStencilView(textureD3D11, &viewDesc, &DepthStencilViewD3D11);
	//D3D11_VERRY(g_pd3dDevice->CreateDepthStencilView(mTextureD3D11.GetTexture(), &viewDesc, &mDepthStencilView));
}

D3D11DepthStencilView::~D3D11DepthStencilView()
{
	SAFE_RELEASE(DepthStencilViewD3D11);
}

void D3D11DepthStencilView::ClearColor( const ColorRGBA& clr )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoundn't call here!", "D3D11DepthStencilView::ClearColor");
}

void D3D11DepthStencilView::ClearDepth( float depth )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	deviceContextD3D11->ClearDepthStencilView(DepthStencilViewD3D11, D3D11_CLEAR_DEPTH, depth, 0);
}

void D3D11DepthStencilView::ClearStencil( uint32_t stencil )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	deviceContextD3D11->ClearDepthStencilView(DepthStencilViewD3D11, D3D11_CLEAR_STENCIL, 0.0f, stencil);
}

void D3D11DepthStencilView::ClearDepthStencil( float depth, uint32_t stencil )
{
	ID3D11DeviceContext* deviceContextD3D11 = gD3D11Device->DeviceContextD3D11;
	deviceContextD3D11->ClearDepthStencilView(DepthStencilViewD3D11, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}












}