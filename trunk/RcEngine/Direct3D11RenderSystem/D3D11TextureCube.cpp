#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Device.h"

namespace RcEngine{

D3D11TextureCube::D3D11TextureCube( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: D3D11Texture(TT_TextureCube, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags),
	  TextureD3D11(nullptr)
{
	assert(width == height);
	mWidth = mHeight = width;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.ArraySize = arraySize;
	texDesc.SampleDesc.Quality = sampleQuality;
	texDesc.SampleDesc.Count = sampleCount;
	texDesc.Format = D3D11Mapping::Mapping(format);
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11Mapping::Mapping(accessHint, texDesc.Usage, texDesc.CPUAccessFlags);

	// Generate mipmaps if enable
	if (mCreateFlags & TexCreate_GenerateMipmaps)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		mMipLevels = RHTexture::CalculateMipmapLevels((std::max)(width, height));
	}
	else
		mMipLevels = numMipMaps;

	if (mCreateFlags & TexCreate_RenderTarget)
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;

	//HRESULT hr = gD3D11Device->DeviceD3D11->CreateTexture2D( &texDesc, NULL, &TextureD3D11);
	D3D11_VERRY(gD3D11Device->DeviceD3D11->CreateTexture2D( &texDesc, NULL, &TextureD3D11));

	/*if (CreateFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = texDesc.Format;

		if (mTextureArraySize <= 1)
		{
			viewDesc.TextureCube.MostDetailedMip = 0;
			viewDesc.TextureCube.MipLevels = mMipLevels;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{	
			viewDesc.TextureCubeArray.MostDetailedMip = 0;
			viewDesc.TextureCubeArray.MipLevels = mMipLevels;
			viewDesc.TextureCubeArray.First2DArrayFace = 0;
			viewDesc.TextureCubeArray.NumCubes = mTextureArraySize;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		}

		D3D11_VERRY(pd3dDevice->CreateShaderResourceView(TextureD3D11, &viewDesc, &ShaderResourceViewD3D11));
	}*/
}

void D3D11TextureCube::MapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{

}

void D3D11TextureCube::UnmapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level )
{

}

}