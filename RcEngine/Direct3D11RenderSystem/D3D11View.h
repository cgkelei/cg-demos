#ifndef D3D11ShaderResourceView_h__
#define D3D11ShaderResourceView_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHView.h>

namespace RcEngine {

class RHBuffer;
class RHTexture;

// Shader Resource View
//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11BufferSRV : public RHBufferSRV
{
public:
	D3D11BufferSRV(const shared_ptr<RHBuffer>& buffer);
	virtual ~D3D11BufferSRV();

public:
	ID3D11ShaderResourceView* ShaderResourceViewD3D11;
};

class _D3D11Export D3D11StructuredBufferSRV : public D3D11BufferSRV
{
public:
	D3D11StructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
};

class _D3D11Export D3D11TextureBufferSRV : public D3D11BufferSRV
{
public:
	D3D11TextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
};

class _D3D11Export D3D11TextureSRV : public RHTextureSRV
{
public:
	D3D11TextureSRV(const shared_ptr<RHTexture>& texture);
	virtual ~D3D11TextureSRV();

public:
	ID3D11ShaderResourceView* ShaderResourceViewD3D11;
};

class _D3D11Export D3D11Texture1DSRV : public D3D11TextureSRV
{
public:
	D3D11Texture1DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
};

class _D3D11Export D3D11Texture2DSRV : public D3D11TextureSRV
{
public:
	D3D11Texture2DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
};

class _D3D11Export D3D11Texture3DSRV : public D3D11TextureSRV
{
public:
	D3D11Texture3DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels);
};

class _D3D11Export D3D11TextureCubeSRV : public D3D11TextureSRV
{
public:
	D3D11TextureCubeSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
};

//// Texture shader resource view creator for each type
//typedef bool (TextureSRVCreator)(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//
//
//template<TextureType type, TextureSRVCreator CreateFunc>
//class _D3D11Export D3D11TextureSRVTemplate : public D3D11TextureSRV
//{
//public:
//	D3D11TextureSRVTemplate(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize)
//		: D3D11TextureSRV(texture)
//	{
//		bool success = CreateFunc(texture, &ShaderResourceViewD3D11);
//	}
//};
//
//// Helper function to create texture shader resource view
//bool CreateTexture1DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTexture2DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTexture3DSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//bool CreateTextureCubeSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV);
//
//typedef D3D11TextureSRVTemplate<TT_Texture1D, CreateTexture1DSRV> D3D11Texture1DSRV;
//typedef D3D11TextureSRVTemplate<TT_Texture2D, CreateTexture2DSRV> D3D11Texture2DSRV;
//typedef D3D11TextureSRVTemplate<TT_Texture3D, CreateTexture3DSRV> D3D11Texture3DSRV;
//typedef D3D11TextureSRVTemplate<TT_TextureCube, CreateTextureCubeSRV> D3D11TextureCubeSRV;


// Unordered Access View
//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11BufferUAV : public RHBufferUAV
{
public:
	D3D11BufferUAV(const shared_ptr<RHBuffer>& buffer);
	virtual ~D3D11BufferUAV();

public:
	ID3D11UnorderedAccessView* UnorderedAccessViewD3D11;
};

class _D3D11Export D3D11TextureUAV : public RHTextureUAV
{
public:
	D3D11TextureUAV(const shared_ptr<RHTexture>& texture);
	virtual ~D3D11TextureUAV();

public:
	ID3D11UnorderedAccessView* UnorderedAccessViewD3D11;
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11TextureBufferUAV : public D3D11BufferUAV
{
public:
	D3D11TextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
};

class _D3D11Export D3D11StructuredBufferUAV : public D3D11BufferUAV
{
public:
	D3D11StructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
};

//////////////////////////////////////////////////////////////////////////
class _D3D11Export D3D11Texture2DUAV : public D3D11TextureUAV
{
public:
	D3D11Texture2DUAV(const shared_ptr<RHTexture>& texture, uint32_t level);
};
}

#endif // D3D11ShaderResourceView_h__