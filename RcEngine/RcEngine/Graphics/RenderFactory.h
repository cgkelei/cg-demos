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

	virtual shared_ptr<Texture> CreateTextureFromFile(const std::string& texFileName, uint32_t accessHint) = 0;


	// Buffers
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<GraphicsBuffer> CreateVertexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;
	virtual shared_ptr<GraphicsBuffer> CreateIndexBuffer(BufferUsage usage, uint32_t accessHint, ElementInitData* initData) = 0;


	// Views
	//-------------------------------------------------------------------------------------------------------
	virtual shared_ptr<RenderView> CreateRenderTargetView2D(const shared_ptr<Texture>& texture, uint32_t arraySize, uint32_t level) = 0;
	virtual shared_ptr<RenderView> CreateDepthStencilView(const shared_ptr<Texture>& texture, uint32_t arraySize, uint32_t level) = 0;
			
	/**
	 * Create a material from file, if the material is already loaded, use the loaded one.
	 * @param matName: material name which is uniquely identify the material.
	 * @param path: the material file location.
	 */
	shared_ptr<Material> CreateMaterialFromFile(const String& matName, const String& path);

	/**
	 * Create a effect from file, if the effect is already loaded, use the loaded one.
	 * @param effectName: effect name which is uniquely identify the effect.
	 * @param effectFile: the effect file location.
	 */
	shared_ptr<Effect> CreateEffectFromFile(const String& effectName, const String& effectFile);
			

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

	shared_ptr<SamplerState> CreateSamplerState( const SamplerStateDesc& desc );

	virtual shared_ptr<Shader> CreateShader(ShaderType type) = 0;
	virtual shared_ptr<ShaderProgram> CreateShaderProgram(Effect& effect) = 0;

protected:

	virtual shared_ptr<DepthStencilState> CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc ) = 0; 
	virtual shared_ptr<BlendState> CreateBlendStateImpl( const BlendStateDesc& desc ) = 0;
	virtual shared_ptr<RasterizerState> CreateRasterizerStateImpl( const RasterizerStateDesc& desc ) = 0;
	virtual shared_ptr<SamplerState> CreateSamplerStateImpl( const SamplerStateDesc& desc ) = 0;

protected:
	typedef unordered_map<String, shared_ptr<Effect> > EffectMap;
	typedef EffectMap::iterator EffectMapIter;

	typedef unordered_map<String, shared_ptr<Material> > MaterialMap;
	typedef MaterialMap::iterator MaterialMapIter;

	std::map<DepthStencilStateDesc, shared_ptr<DepthStencilState> > mDepthStecilStatePool;
	std::map<RasterizerStateDesc, shared_ptr<RasterizerState> > mRasterizerStatePool;
	//std::map<SamplerStateDesc, shared_ptr<SamplerState> > mSamplerStatePool;
	std::map<BlendStateDesc, shared_ptr<BlendState> > mBlendStatePool;


	EffectMap mEffectPool;
	MaterialMap mMaterialPool;
};



} // Namespace RcEngine


#endif // RenderFactory_h__