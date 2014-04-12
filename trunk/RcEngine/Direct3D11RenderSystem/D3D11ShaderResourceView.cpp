#include "D3D11ShaderResourceView.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"

namespace RcEngine {

ID3D11Device* g_pD3D11Device;

D3D11BufferSRV::D3D11BufferSRV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferSRV(buffer),
	  ShaderResourceViewD3D11(nullptr)
{

}

D3D11BufferSRV::~D3D11BufferSRV()
{
	SAFE_RELEASE(ShaderResourceViewD3D11);
}

D3D11StructuredBufferSRV::D3D11StructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: D3D11BufferSRV(buffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = elementCount;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = g_pD3D11Device->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureBufferSRV::D3D11TextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: D3D11BufferSRV(buffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = elementCount;
	
	//assert(PixelFormatUtils::GetNumElemBytes(format) * elementCount == buffer->GetBufferSize());

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = g_pD3D11Device->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
}

bool CreateTexture1DSRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	assert(texture.GetTextureType() == TT_Texture1D && texture.GetTextureArraySize() <= 1);
	
	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		viewDesc.Texture1D.MostDetailedMip = 0;
		viewDesc.Texture1D.MipLevels = texture.GetMipLevels();
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

		const D3D11Texture1D* textureD3D11 = static_cast<const D3D11Texture1D*>(&texture);
		HRESULT hr = g_pD3D11Device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture1DArraySRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	assert(texture.GetTextureType() == TT_Texture1D && texture.GetTextureArraySize() > 1);
	
	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		viewDesc.Texture1DArray.FirstArraySlice = 0;
		viewDesc.Texture1DArray.ArraySize = texture.GetTextureArraySize();
		viewDesc.Texture1DArray.MostDetailedMip = 0;
		viewDesc.Texture1DArray.MipLevels = texture.GetMipLevels();
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;

		const D3D11Texture1D* textureD3D11 = static_cast<const D3D11Texture1D*>(&texture);
		HRESULT hr = g_pD3D11Device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture2DSRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	assert(texture.GetTextureType() == TT_Texture2D && texture.GetTextureArraySize() <= 1);
	
	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		if (texture.GetSampleCount() > 1 && (createFlags & TexCreate_RenderTarget))
		{
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			viewDesc.Texture2D.MostDetailedMip = 0;
			viewDesc.Texture2D.MipLevels = texture.GetMipLevels();
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}

		const D3D11Texture2D* textureD3D11 = static_cast<const D3D11Texture2D*>(&texture);
		HRESULT hr = device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}
	
	return success;
}

bool CreateTexture2DArraySRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	assert(texture.GetTextureType() == TT_Texture2D && texture.GetTextureArraySize() > 1);

	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		if (texture.GetSampleCount() > 1 && (createFlags & TexCreate_RenderTarget))
		{
			viewDesc.Texture2DMSArray.ArraySize = texture.GetTextureArraySize();
			viewDesc.Texture2DMSArray.FirstArraySlice = 0;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		}
		else
		{
			viewDesc.Texture2DArray.FirstArraySlice = 0;
			viewDesc.Texture2DArray.ArraySize = texture.GetTextureArraySize();
			viewDesc.Texture2DArray.MostDetailedMip = 0;
			viewDesc.Texture2DArray.MipLevels = texture.GetMipLevels();
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		}

		const D3D11Texture2D* textureD3D11 = static_cast<const D3D11Texture2D*>(&texture);
		HRESULT hr = device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture3DSRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	return false;
}

bool CreateTextureCubeSRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	assert(texture.GetTextureType() == TT_TextureCube && texture.GetTextureArraySize() <= 1);

	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texture.GetMipLevels();
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

		const D3D11TextureCube* textureD3D11 = static_cast<const D3D11TextureCube*>(&texture);
		HRESULT hr = device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTextureCubeArraySRV( ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV)
{
	assert(texture.GetTextureType() == TT_TextureCube && texture.GetTextureArraySize() > 1);

	bool success = false;

	uint32_t createFlags = texture.GetCreateFlags();
	if (createFlags & TexCreate_ShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
		viewDesc.Format = D3D11Mapping::Mapping(texture.GetTextureFormat());

		viewDesc.TextureCubeArray.MostDetailedMip = 0;
		viewDesc.TextureCubeArray.MipLevels = texture.GetMipLevels();
		viewDesc.TextureCubeArray.First2DArrayFace = 0;
		viewDesc.TextureCubeArray.NumCubes = texture.GetTextureArraySize();
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;

		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = 
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

		const D3D11TextureCube* textureD3D11 = static_cast<const D3D11TextureCube*>(&texture);
		HRESULT hr = device->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}




}