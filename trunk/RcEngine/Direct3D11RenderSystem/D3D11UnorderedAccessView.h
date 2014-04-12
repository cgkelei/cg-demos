#ifndef D3D11UnorderedAccessView_h__
#define D3D11UnorderedAccessView_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHView.h>

namespace RcEngine {

class RHBuffer;

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

public:
	ID3D11UnorderedAccessView* UnorderedAccessViewD3D11;
};


}


#endif // D3D11UnorderedAccessView_h__