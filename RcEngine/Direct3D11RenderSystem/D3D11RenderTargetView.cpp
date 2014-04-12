#include "D3D11RenderView.h"
#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {


D3D11RenderTargetView2D::D3D11RenderTargetView2D( Texture& texture, uint32_t arrIndex, uint32_t level )
	: mTextureD3D11(*(static_cast_checked<D3D11Texture2D*>(&texture))),
	  mArrIndex(arrIndex),
	  mLevel(level),
	  mRenderTargetView2D(nullptr)
{
	assert(mTextureD3D11.CreateFlags & TexCreate_RenderTarget);

	ID3D11Device*           g_pd3dDevice = NULL;

	D3D11_RENDER_TARGET_VIEW_DESC  viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(mTextureD3D11.GetTextureFormat());

	uint32_t textureArraySize = mTextureD3D11.GetTextureArraySize();
	if (textureArraySize <= 1)
	{
		if (mTextureD3D11.GetSampleCount() > 1)
		{
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			viewDesc.Texture2D.MipSlice = level;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		}
	}
	else
	{
		if (mTextureD3D11.GetSampleCount() > 1)
		{
			viewDesc.Texture2DMSArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DMSArray.ArraySize = 1;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		}
		else
		{
			viewDesc.Texture2DArray.FirstArraySlice = arrIndex;
			viewDesc.Texture2DArray.ArraySize = 1;
			viewDesc.Texture2DArray.MipSlice = level;
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		}
	}

	D3D11_VERRY(g_pd3dDevice->CreateRenderTargetView(mTextureD3D11.GetTexture(), &viewDesc, &mRenderTargetView2D));
}


D3D11RenderTargetView2D::~D3D11RenderTargetView2D()
{
	SAFE_RELEASE(mRenderTargetView2D);
}



}