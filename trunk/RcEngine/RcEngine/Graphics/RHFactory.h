#ifndef RHRenderFactory_h__
#define RHRenderFactory_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/RHState.h>

namespace RcEngine {

struct RHBlendStateDesc;
struct RHSamplerStateDesc;
struct RHRasterizerStateDesc;
struct RHDepthStencilStateDesc;

class RHBlendState;
class RHSamplerState;
class RHRasterizerState;
class RHDepthStencilSate;

class RHShader;

class RHBuffer;
class RHTexture;

class RHBufferSRV;
class RHBufferUAV;
class RHTextureSRV;
class RHTextureUAV;

struct ElementInitData;
struct ShaderMacro;

class _ApiExport RHRenderFactory
{
public:
	RHRenderFactory();
	virtual ~RHRenderFactory() {}

	// States
	shared_ptr<RHBlendState> CreateBlendState( const RHBlendStateDesc& desc );
	shared_ptr<RHSamplerState> CreateSamplerState( const RHSamplerStateDesc& desc );
	shared_ptr<RHRasterizerState> CreateRasterizerState( const RHRasterizerStateDesc& desc );
	shared_ptr<RHDepthStencilSate> CreateDepthStencilState( const RHDepthStencilStateDesc& desc ); 
	
	// Buffer resource
	virtual shared_ptr<RHBuffer> CreateVertexBuffer(uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;
	virtual shared_ptr<RHBuffer> CreateIndexBuffer(uint32_t accessHint, uint32_t createFlags,ElementInitData* initData) = 0;
	virtual shared_ptr<RHBuffer> CreateUniformBuffer(uint32_t accessHint, uint32_t createFlags,ElementInitData* initData) = 0;
	virtual shared_ptr<RHBuffer> CreateTextureBuffer(uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;
	
	/** ElementInitData's slicePitch for StructureByteStride */
	virtual shared_ptr<RHBuffer> CreateStructuredBuffer(uint32_t accessHint, uint32_t createFlags, ElementInitData* initData) = 0;

	// Texture resource
	virtual shared_ptr<RHTexture> CreateTexture1D(
		uint32_t width,
		PixelFormat format, 
		uint32_t arrSize, 
		uint32_t numMipMaps,
		uint32_t sampleCount, 
		uint32_t sampleQuality,
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData) = 0;

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
		ElementInitData* initData) = 0;

	virtual shared_ptr<RHTexture> CreateTexture3D(
		uint32_t width, 
		uint32_t height,
		uint32_t depth, 
		PixelFormat format, 
		uint32_t arraySize,
		uint32_t numMipMaps, 
		uint32_t sampleCount,
		uint32_t sampleQuality, 
		uint32_t accessHint, 
		uint32_t createFlags,
		ElementInitData* initData) = 0;

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
		ElementInitData* initData) = 0;

	// Shader resource
	virtual shared_ptr<RHShader> CreateShader(ShaderType type) = 0;
	
	// Utility function
	shared_ptr<RHTextureSRV> LoadTextureSRVFromFile(const String& filename);
	shared_ptr<RHTexture> LoadTextureFromFile(const String& filename);

	shared_ptr<RHShader> LoadShaderFromFile(
		ShaderType shaderType, 
		const String& filename, 
		const vector<ShaderMacro>& macros,
		const String& entryPoint = "");

	// Shader resource view
	virtual shared_ptr<RHTextureSRV> CreateTextureSRV(const shared_ptr<RHTexture>& texture) = 0;
	virtual shared_ptr<RHBufferUAV> CreateStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount) = 0;
	virtual shared_ptr<RHBufferUAV> CreateTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format) = 0;

	// Unordered access view
	virtual shared_ptr<RHTextureUAV> CreateTextureUAV(const shared_ptr<RHTexture>& texture) = 0;
	virtual shared_ptr<RHBufferUAV> CreateStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount) = 0;
	virtual shared_ptr<RHBufferUAV> CreateTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format) = 0;

protected:
	virtual shared_ptr<RHBlendState> CreateBlendStateImpl(const RHBlendStateDesc& desc) = 0;
	virtual shared_ptr<RHSamplerState> CreateSamplerStateImpl(const RHSamplerStateDesc& desc) = 0;
	virtual shared_ptr<RHRasterizerState> CreateRasterizerStateImpl(const RHRasterizerStateDesc& desc) = 0;
	virtual shared_ptr<RHDepthStencilSate> CreateDepthStencilStateImpl(const RHDepthStencilStateDesc& desc) = 0;

protected:
	std::map<String, shared_ptr<RHShader> > mShaderPool;
	std::map<RHBlendStateDesc, shared_ptr<BlendState> > mBlendStatePool;
	std::map<RHSamplerStateDesc, shared_ptr<SamplerState> > mSamplerStatePool;
	std::map<RHRasterizerStateDesc, shared_ptr<RasterizerState> > mRasterizerStatePool;
	std::map<RHDepthStencilStateDesc, shared_ptr<DepthStencilState> > mDepthStecilStatePool;
};









}


#endif // RHRenderFactory_h__
