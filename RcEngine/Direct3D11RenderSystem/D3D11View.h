#ifndef D3D11ShaderResourceView_h__
#define D3D11ShaderResourceView_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHView.h>

namespace RcEngine {

class RHBuffer;
class RHTexture;

class _D3D11Export D3D11BufferSRV : public RHBufferSRV
{
public:
	D3D11BufferSRV(const shared_ptr<RHBuffer>& buffer);
	virtual ~D3D11BufferSRV();

public:
	ID3D11ShaderResourceView* ShaderResourceViewD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport D3D11StructuredBufferSRV : public D3D11BufferSRV
{
public:
	D3D11StructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport D3D11TextureBufferSRV : public D3D11BufferSRV
{
public:
	D3D11TextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11TextureSRV : public RHTextureSRV
{
public:
	D3D11TextureSRV(const shared_ptr<RHTexture>& texture);
	virtual ~D3D11TextureSRV();

public:
	ID3D11ShaderResourceView* ShaderResourceViewD3D11;
};

// Texture shader resource view creator for each type
typedef bool (TextureSRVCreator)(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);


template<D3D11_SRV_DIMENSION SRV, TextureSRVCreator CreateFunc>
class _ApiExport D3D11TextureSRVTemplate : public D3D11TextureSRV
{
public:
	D3D11TextureSRVTemplate(const shared_ptr<RHTexture>& texture)
		: D3D11TextureSRV(texture)
	{
		ID3D11Device* device;
		ShaderResourceViewD3D11 = CreateFunc(device, *texture);
	}
};

// Helper function to create texture shader resource view
bool CreateTexture1DSRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTexture1DArraySRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTexture2DSRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTexture2DArraySRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTexture3DSRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTextureCubeSRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);
bool CreateTextureCubeArraySRV(ID3D11Device* device, const RHTexture& texture, ID3D11ShaderResourceView** pTextureSRV);

typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURE1D, CreateTexture1DSRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURE1DARRAY, CreateTexture1DArraySRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURE2D, CreateTexture2DSRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURE1DARRAY, CreateTexture2DArraySRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURE3D, CreateTexture3DSRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURECUBE, CreateTextureCubeSRV> D3D11Texture1DSRV;
typedef D3D11TextureSRVTemplate<D3D11_SRV_DIMENSION_TEXTURECUBEARRAY, CreateTextureCubeArraySRV> D3D11Texture1DSRV;

}

#endif // D3D11ShaderResourceView_h__