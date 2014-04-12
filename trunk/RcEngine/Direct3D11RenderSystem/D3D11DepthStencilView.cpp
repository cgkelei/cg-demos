#include "D3D11RenderView.h"
#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {

D3D11DepthStencilView::D3D11DepthStencilView( Texture& texture, uint32_t arrIndex, uint32_t level )
	: mTextureD3D11(*(static_cast_checked<D3D11Texture2D*>(&texture))),
	  mArrIndex(arrIndex),
	  mLevel(level)
{
	uint32_t createFlags = mTextureD3D11.GetCreateFlags();
	assert(createFlags & TexCreate_DepthStencilTarget);

	ID3D11Device*           g_pd3dDevice = NULL;

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(mTextureD3D11.GetTextureFormat());

	uint32_t textureArraySize = mTextureD3D11.GetTextureArraySize();
	if (textureArraySize <= 1)
	{
		if (mTextureD3D11.GetSampleCount() > 1)
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
		if (mTextureD3D11.GetSampleCount() > 1)
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

	D3D11_VERRY(g_pd3dDevice->CreateDepthStencilView(mTextureD3D11.GetTexture(), &viewDesc, &mDepthStencilView));
}

D3D11DepthStencilView::~D3D11DepthStencilView()
{
	SAFE_RELEASE(mDepthStencilView);
}













}