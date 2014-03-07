#pragma once
#ifndef OpenGLRenderFactory_h__
#define OpenGLRenderFactory_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/RenderFactory.h>


namespace RcEngine {


class _OpenGLExport OpenGLRenderFactory : public RenderFactory
{
public:
	OpenGLRenderFactory(void);
	~OpenGLRenderFactory(void);
	
	// Texture
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<Texture> CreateTexture1D(uint32_t width, PixelFormat format, uint32_t arrSize, 
		uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, 
		ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat format, 
		uint32_t arrSize, uint32_t numMipMaps,  uint32_t sampleCount, uint32_t sampleQuality, 
		uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTexture3D(uint32_t width, uint32_t height, uint32_t depth, 
		PixelFormat format, uint32_t arraySize, uint32_t numMipMaps,  uint32_t sampleCount,
		uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	virtual shared_ptr<Texture> CreateTextureCube(uint32_t width, uint32_t height, PixelFormat format,
		uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality,
		uint32_t accessHint, ElementInitData* initData);

	// Views
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<RenderView> CreateRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level);
	virtual shared_ptr<RenderView> CreateDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level);

	virtual shared_ptr<FrameBuffer> CreateFrameBuffer(uint32_t width, uint32_t height);
	
	// Buffer
	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData);
	virtual shared_ptr<GraphicsBuffer> CreateConstantBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData);

	// Shader
	virtual shared_ptr<Shader> CreateShader(ShaderType type);
	virtual shared_ptr<ShaderProgram> CreateShaderProgram(Effect& effect);

	// 
	virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, uint32_t accessHint);
	virtual shared_ptr<Texture> CreateTextureArrayFromFile( const vector<String>& textures, uint32_t accessHint = 0);
	virtual void SaveTexture2D( const String& texFile, const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level);
	
protected:
	virtual shared_ptr<DepthStencilState> CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc ); 
	virtual shared_ptr<BlendState> CreateBlendStateImpl( const BlendStateDesc& desc );
	virtual shared_ptr<RasterizerState> CreateRasterizerStateImpl( const RasterizerStateDesc& desc );
	virtual shared_ptr<SamplerState> CreateSamplerStateImpl( const SamplerStateDesc& desc );
	


private:

};

}

#endif // OpenGLRenderFactory_h__