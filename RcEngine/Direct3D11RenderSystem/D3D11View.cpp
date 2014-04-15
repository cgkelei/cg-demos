#include "D3D11View.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace RcEngine {

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
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
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
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
}

bool CreateTexture1DSRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture1DArraySRV(const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture2DSRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}
	
	return success;
}

bool CreateTexture2DArraySRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTexture3DSRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
{
	return false;
}

bool CreateTextureCubeSRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV )
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

bool CreateTextureCubeArraySRV( const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV)
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
		HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
		success = SUCCEEDED(hr);
	}

	return success;
}

//////////////////////////////////////////////////////////////////////////
D3D11BufferUAV::D3D11BufferUAV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferUAV(buffer),
	UnorderedAccessViewD3D11(nullptr)
{

}

D3D11BufferUAV::~D3D11BufferUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}


D3D11TextureBufferUAV::D3D11TextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}


D3D11StructuredBufferUAV::D3D11StructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureUAV::D3D11TextureUAV( const shared_ptr<RHTexture>& texture )
	: RHTextureUAV(texture),
	UnorderedAccessViewD3D11(nullptr)
{

}

D3D11TextureUAV::~D3D11TextureUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}

D3D11Texture2DUAV::D3D11Texture2DUAV( const shared_ptr<RHTexture>& texture, uint32_t level )
	: D3D11TextureUAV(texture)
{
	D3D11Texture2D* textureD3D11 = (static_cast<D3D11Texture2D*>(texture.get()));

	uint32_t createFlags = textureD3D11->GetCreateFlags();

	assert(textureD3D11->GetTextureArraySize() <= 1);
	assert(createFlags & TexCreate_UAV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = level;

	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &desc, &UnorderedAccessViewD3D11);
}



}