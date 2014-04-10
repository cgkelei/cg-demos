#pragma once
#ifndef RenderFactory_h__
#define RenderFactory_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/BlendState.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/SamplerState.h>
#include <Graphics/Shader.h>

namespace RcEngine {

class RenderEffect;
class Shader;
class ShaderProgram;

class _ApiExport RenderFactory
{
public:
	RenderFactory(void);
	~RenderFactory(void);

	// Texture
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<Texture> CreateTexture1D(uint32_t width, PixelFormat format, uint32_t arrSize, 
		uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, 
		ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTexture2D(uint32_t width, uint32_t height, PixelFormat format, 
		uint32_t arrSize, uint32_t numMipMaps,  uint32_t sampleCount, uint32_t sampleQuality, 
		uint32_t accessHint, ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTexture3D(uint32_t width, uint32_t height, uint32_t depth, 
		PixelFormat format, uint32_t arraySize, uint32_t numMipMaps,  uint32_t sampleCount,
		uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData) = 0;

	virtual shared_ptr<Texture> CreateTextureCube(uint32_t width, uint32_t height, PixelFormat format,
		uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality,
		uint32_t accessHint, ElementInitData* initData) = 0;
	
	// Buffers
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateConstantBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;

	// Create structured buffer, for D3D11's ID3D11Buffer, for OpenGL's shader storage object
	virtual shared_ptr<GraphicsBuffer> CreateStructuredBuffer(ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateStructuredUAV(const GraphicsBuffer& buffer) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateStructuredSRV(const GraphicsBuffer& buffer) = 0;

	// FrameBuffers
	//-------------------------------------------------------------------------------------------------------	
	virtual shared_ptr<FrameBuffer> CreateFrameBuffer(uint32_t width, uint32_t height) = 0;

	// Views
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<RenderView> CreateRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level) = 0;
	virtual shared_ptr<RenderView> CreateDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level) = 0;
			
	/**
	 * Create an vertex declaration object to describe the input-buffer data.
	 */
	shared_ptr<VertexDeclaration> CreateVertexDeclaration(const std::vector<VertexElement>& elems);
	shared_ptr<VertexDeclaration> CreateVertexDeclaration(VertexElement* elems, uint32_t count);

	/**
	 * Create a depth-stencil state object that encapsulates depth-stencil test information.
	 */
	shared_ptr<DepthStencilState> CreateDepthStencilState( const DepthStencilStateDesc& desc ); 
	
	/**
	 * Create a blend-state object that encapsulates blend state.
	 */
	shared_ptr<BlendState> CreateBlendState( const BlendStateDesc& desc );

	/**
	 * Create a rasterizer state object that tells the rasterizer stage how to behave.
	 */
	shared_ptr<RasterizerState> CreateRasterizerState( const RasterizerStateDesc& desc );

	/**
	 * Create HLSL/GLSL shader, Set BinaryHint to true to use pre-compiled shader bytecode.
	 */
	virtual void SetShaderBinaryHint(bool bEnable) = 0;
	shared_ptr<Shader> CreateShader(ShaderType shaderType, const String& filename, const std::vector<ShaderMacro>& macros, const String& entryPoint = "");

	/**
	 * Create a sampler state object.
	 */
	shared_ptr<SamplerState> CreateSamplerState( const SamplerStateDesc& desc );

	virtual shared_ptr<Shader> CreateShader(ShaderType type) = 0;
	virtual shared_ptr<ShaderProgram> CreateShaderProgram(Effect& effect) = 0;
	
	/**
	 * Todo
	 * Implement texture load and save
	 */
	virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, uint32_t accessHint = 0) = 0;
	virtual shared_ptr<Texture> CreateTextureArrayFromFile( const vector<String>& textures, uint32_t accessHint = 0) = 0;
	virtual void SaveTexture2D( const String& texFile, const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level) = 0;

protected:

	virtual shared_ptr<BlendState> CreateBlendStateImpl( const BlendStateDesc& desc ) = 0;
	virtual shared_ptr<SamplerState> CreateSamplerStateImpl( const SamplerStateDesc& desc ) = 0;
	virtual shared_ptr<RasterizerState> CreateRasterizerStateImpl( const RasterizerStateDesc& desc ) = 0;
	virtual shared_ptr<DepthStencilState> CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc ) = 0; 

	virtual shared_ptr<Shader> CreateShaderImpl(ShaderType shaderType) = 0;

protected:
	bool mShaderBinaryHint;

	std::map<String, shared_ptr<Shader> > mShaderPool;
	std::map<BlendStateDesc, shared_ptr<BlendState> > mBlendStatePool;
	std::map<SamplerStateDesc, shared_ptr<SamplerState> > mSamplerStatePool;
	std::map<RasterizerStateDesc, shared_ptr<RasterizerState> > mRasterizerStatePool;
	std::map<DepthStencilStateDesc, shared_ptr<DepthStencilState> > mDepthStecilStatePool;
};



} // Namespace RcEngine


#endif // RenderFactory_h__