#ifndef D3D11RenderFactory_h__
#define D3D11RenderFactory_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/RHFactory.h>

namespace RcEngine {

class _D3D11Export D3D11Factory : public RHFactory 
{
public:
	D3D11Factory();

	virtual shared_ptr<RHVertexDeclaration> CreateVertexDeclaration(RHVertexElement* elems, uint32_t count);

	virtual shared_ptr<RHFrameBuffer> CreateFrameBuffer(uint32_t width, uint32_t height);

	// Buffer resource
	virtual shared_ptr<RHBuffer> CreateVertexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateIndexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateUniformBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateTextureBuffer(PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);

	// Texture resource
	virtual shared_ptr<RHTexture> CreateTexture1D(
		uint32_t width,
		PixelFormat format, 
		uint32_t arrSize, 
		uint32_t numMipMaps,
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData);

	virtual shared_ptr<RHTexture> CreateTexture2D(
		uint32_t width,
		uint32_t height,
		PixelFormat format, 
		uint32_t arrSize,
		uint32_t numMipMaps,
		uint32_t sampleCount,
		uint32_t sampleQuality, 
		uint32_t accessHint,
		uint32_t createFlags,
		ElementInitData* initData);

	virtual shared_ptr<RHTexture> CreateTexture3D(
		uint32_t width, 
		uint32_t height,
		uint32_t depth, 
		PixelFormat format, 
		uint32_t numMipMaps, 
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData);

	virtual shared_ptr<RHTexture> CreateTextureCube(
		uint32_t width, 
		uint32_t height,
		PixelFormat format,
		uint32_t arraySize, 
		uint32_t numMipMaps, 
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t createFlags,
		ElementInitData* initData);

	// Shader resource
	virtual shared_ptr<RHShader> CreateShader(ShaderType type);
	virtual shared_ptr<RHShaderPipeline> CreateShaderPipeline(Effect& effect);

	// Shader resource view
	virtual shared_ptr<RHShaderResourceView> CreateStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride);
	virtual shared_ptr<RHShaderResourceView> CreateTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format);

	virtual shared_ptr<RHUnorderedAccessView> CreateStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride);
	virtual shared_ptr<RHUnorderedAccessView> CreateTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format);

	virtual shared_ptr<RHShaderResourceView> CreateTexture1DSRV(const shared_ptr<RHTexture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	
	virtual shared_ptr<RHShaderResourceView> CreateTexture2DSRV(const shared_ptr<RHTexture>& texture,
		uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<RHShaderResourceView> CreateTexture3DSRV(const shared_ptr<RHTexture>& texture);

	virtual shared_ptr<RHShaderResourceView> CreateTextureCubeSRV(const shared_ptr<RHTexture>& texture);

	virtual shared_ptr<RHUnorderedAccessView> CreateTexture1DUAV(const shared_ptr<RHTexture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<RHUnorderedAccessView> CreateTexture2DUAV(const shared_ptr<RHTexture>& texture,
		uint32_t level, uint32_t firstArraySlice, uint32_t arraySize);

	virtual shared_ptr<RHUnorderedAccessView> CreateTexture3DUAV(const shared_ptr<RHTexture>& texture);
	
	virtual shared_ptr<RHUnorderedAccessView> CreateTextureCubeUAV(const shared_ptr<RHTexture>& texture);

	// Render target view
	virtual shared_ptr<RHRenderView> CreateDepthStencilView(const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RHRenderView> CreateRenderTargetView2D(const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RHRenderView> CreateRenderTargetViewArray(const shared_ptr<RHTexture>& texture, uint32_t level);

protected:
	virtual shared_ptr<RHBlendState> CreateBlendStateImpl(const RHBlendStateDesc& desc);
	virtual shared_ptr<RHSamplerState> CreateSamplerStateImpl(const RHSamplerStateDesc& desc);
	virtual shared_ptr<RHRasterizerState> CreateRasterizerStateImpl(const RHRasterizerStateDesc& desc);
	virtual shared_ptr<RHDepthStencilState> CreateDepthStencilStateImpl(const RHDepthStencilStateDesc& desc);
};

}


#endif // D3D11RenderFactory_h__