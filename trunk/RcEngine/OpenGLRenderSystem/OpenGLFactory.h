#pragma once
#ifndef OpenGLRenderFactory_h__
#define OpenGLRenderFactory_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/RHFactory.h>

namespace RcEngine {

class _OpenGLExport OpenGLFactory : public RHFactory
{
public:
	OpenGLFactory(void) {}
	~OpenGLFactory(void) {}
	
	// Buffer resource
	virtual shared_ptr<RHBuffer> CreateVertexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateIndexBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateUniformBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateTextureBuffer(uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);
	virtual shared_ptr<RHBuffer> CreateStructuredBuffer(uint32_t buffreSize, uint32_t strutureStride, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData);

	// Shader
	virtual shared_ptr<RHShader> CreateShader(ShaderType type);

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

	// Shader resource view
	virtual shared_ptr<RHTextureSRV> CreateTextureSRV(const shared_ptr<RHTexture>& texture);
	virtual shared_ptr<RHBufferSRV> CreateStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
	virtual shared_ptr<RHBufferSRV> CreateTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);

	// Unordered access view
	virtual shared_ptr<RHTextureUAV> CreateTextureUAV(const shared_ptr<RHTexture>& texture);
	virtual shared_ptr<RHBufferUAV> CreateStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
	virtual shared_ptr<RHBufferUAV> CreateTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);

	// Render target view
	virtual shared_ptr<RHRenderView> CreateRenderTargetView2D(const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RHRenderView> CreateDepthStencilView(const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RHRenderView> CreateRenderTargetViewArray(const shared_ptr<RHTexture>& texture, uint32_t level);

protected:
	virtual shared_ptr<RHBlendState> CreateBlendStateImpl(const RHBlendStateDesc& desc);
	virtual shared_ptr<RHSamplerState> CreateSamplerStateImpl(const RHSamplerStateDesc& desc);
	virtual shared_ptr<RHRasterizerState> CreateRasterizerStateImpl(const RHRasterizerStateDesc& desc);
	virtual shared_ptr<RHDepthStencilState> CreateDepthStencilStateImpl(const RHDepthStencilStateDesc& desc);
};

}

#endif // OpenGLRenderFactory_h__