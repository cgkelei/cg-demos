#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {

D3D11Texture2D::D3D11Texture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
							   uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint,
							   uint32_t flags, ElementInitData* initData )
	: D3D11TextureBase(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags),
	  mTexture2D(nullptr)
{
	mWidth = width;
	mHeight = height;
	
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.ArraySize = arraySize;
	texDesc.SampleDesc.Quality = sampleQuality;
	texDesc.SampleDesc.Count = sampleCount;
	texDesc.Format = D3D11Mapping::Mapping(format);
	
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	D3D11Mapping::MapUsage(accessHint, texDesc.Usage, texDesc.CPUAccessFlags);

	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	
	// Generate mipmaps if enable
	if (mFlags & TexCreate_GenerateMipmaps)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		// May defer it to DeviceContext::GenerateMips called.
		mMipLevels = Texture::CalculateMipmapLevels((std::max)(width, height));
	}
	else
		mMipLevels = numMipMaps;

	if (mFlags & TexCreate_UAV)
		texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	if (mFlags & TexCreate_RenderTarget)
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	else if (mFlags & TexCreate_DepthStencilTarget)
		texDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;

	/////////////////////////
	ID3D11Device* pd3dDevice;

	if (initData)
	{
		uint32_t numSubResource = mTextureArraySize * numMipMaps;
		vector<D3D11_SUBRESOURCE_DATA> subResourceData(numSubResource);

		for (uint32_t arrayIdx = 0; arrayIdx < mTextureArraySize; ++arrayIdx)
		{
			for (uint32_t level = 0; level < numMipMaps; ++level)
			{
				uint32_t index = arrayIdx*numMipMaps+level;
				subResourceData[index].pSysMem = initData[index].pData;
				subResourceData[index].SysMemPitch = initData[index].rowPitch;
			}
		}

		D3D11_VERRY(pd3dDevice->CreateTexture2D( &texDesc, &subResourceData[0], &mTexture2D));
	}
	else 
		D3D11_VERRY(pd3dDevice->CreateTexture2D( &texDesc, NULL, &mTexture2D));
	
	

	// Create shader resource view
	if (mFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = texDesc.Format;

		if (mTextureArraySize <= 1)
		{
			if (mSampleCount > 1 && (mFlags & TexCreate_RenderTarget))
			{
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				viewDesc.Texture2D.MostDetailedMip = 0;
				viewDesc.Texture2D.MipLevels = numMipMaps;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			}
		}
		else
		{
			if (sampleCount > 1 && (mFlags & TexCreate_RenderTarget))
			{
				viewDesc.Texture2DMSArray.ArraySize = mTextureArraySize;
				viewDesc.Texture2DMSArray.FirstArraySlice = 0;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			}
			else
			{
				viewDesc.Texture2DArray.FirstArraySlice = 0;
				viewDesc.Texture2DArray.ArraySize = mTextureArraySize;
				viewDesc.Texture2DArray.MostDetailedMip = 0;
				viewDesc.Texture2DArray.MipLevels = numMipMaps;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			}	
		}
	
		D3D11_VERRY(pd3dDevice->CreateShaderResourceView(mTexture2D, &viewDesc, &mShaderResourceView));
	}
}

D3D11Texture2D::~D3D11Texture2D()
{
	SAFE_RELEASE(mTexture2D);
}


void D3D11Texture1D::Map1D( uint32_t arrayIndex, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{

}

void D3D11Texture1D::Unmap1D( uint32_t arrayIndex, uint32_t level )
{

}


}