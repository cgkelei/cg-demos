#include "D3D11Factory.h"
#include "D3D11Buffer.h"
#include "D3D11Texture.h"
#include "D3D11FrameBuffer.h"
#include "D3D11View.h"
#include "D3D11Shader.h"
#include "D3D11State.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Factory::D3D11Factory( ID3D11Device* deviceD3D11 )
	: RHFactory(),
	  mDeviceD3D11(deviceD3D11)
{

}

shared_ptr<RHBuffer> D3D11Factory::CreateVertexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<RHBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Vertex | createFlags), 0, initData) );
}

shared_ptr<RHBuffer> D3D11Factory::CreateIndexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return shared_ptr<RHBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Index | createFlags), 0, initData) );
}

shared_ptr<RHBuffer> D3D11Factory::CreateUniformBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return shared_ptr<RHBuffer>(
		new D3D11Buffer(buffreSize, accessHint, (BufferCreate_Uniform | createFlags), 0, initData) );
}

shared_ptr<RHBuffer> D3D11Factory::CreateTextureBuffer( PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(format);
	return shared_ptr<RHBuffer>(
		new D3D11Buffer(texelSize * elementCount, accessHint, (BufferCreate_Texture | createFlags), texelSize, initData) );
}

shared_ptr<RHBuffer> D3D11Factory::CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData)
{
	return shared_ptr<RHBuffer>(
		new D3D11Buffer(strutureStride * elementCount, accessHint, (BufferCreate_Structured | createFlags), strutureStride, initData) );
}

shared_ptr<RHTexture> D3D11Factory::CreateTexture1D( uint32_t width, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<RHTexture>(
		new D3D11Texture1D(format, arrSize, numMipMaps, width, accessHint, createFlags, initData) );
}

shared_ptr<RHTexture> D3D11Factory::CreateTexture2D( uint32_t width, uint32_t height, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<RHTexture>(
		new D3D11Texture2D(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData) );
}

shared_ptr<RHTexture> D3D11Factory::CreateTexture3D( uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<RHTexture>(
		new D3D11Texture3D(format, numMipMaps, width, height, depth, accessHint, createFlags, initData) );
}

shared_ptr<RHTexture> D3D11Factory::CreateTextureCube( uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<RHTexture>(
		new D3D11TextureCube(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData) );
}

shared_ptr<RHShader> D3D11Factory::CreateShader( ShaderType type )
{
	switch (type)
	{
	case ST_Vertex:		return shared_ptr<RHShader>( new D3D11VertexShader() );
	case ST_Hull:		return shared_ptr<RHShader>( new D3D11HullShader() );
	case ST_Domain:     return shared_ptr<RHShader>( new D3D11DomainShader() );
	case ST_Geomerty:   return shared_ptr<RHShader>( new D3D11GeometryShader() );
	case ST_Pixel:		return shared_ptr<RHShader>( new D3D11PixelShader() );
	case ST_Compute:	return shared_ptr<RHShader>( new D3D11ComputeShader() );
	default:
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Undefined shader type", "D3D11Factory::CreateShader");
	}
}

shared_ptr<RHBufferSRV> D3D11Factory::CreateStructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
{
	return shared_ptr<RHBufferSRV>( new D3D11StructuredBufferSRV(buffer, elementCount) );
}

shared_ptr<RHBufferSRV> D3D11Factory::CreateTextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<RHBufferSRV>( new D3D11TextureBufferSRV(buffer, elementCount, format) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTexture1DSRV( const shared_ptr<RHTexture>& texture )
{
	return shared_ptr<RHTextureSRV>( new D3D11Texture1DSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTexture1DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<RHTextureSRV>( new D3D11Texture1DSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTexture2DSRV( const shared_ptr<RHTexture>& texture )
{
	return shared_ptr<RHTextureSRV>( new D3D11Texture2DSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTexture2DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<RHTextureSRV>( new D3D11Texture2DSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTexture3DSRV( const shared_ptr<RHTexture>& texture )
{
	return shared_ptr<RHTextureSRV>( new D3D11Texture3DSRV(texture, 0, texture->GetMipLevels()) );
}

shared_ptr<RHTextureSRV> D3D11Factory::CreateTextureCubeSRV( const shared_ptr<RHTexture>& texture )
{
	return shared_ptr<RHTextureSRV>( new D3D11TextureCubeSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<RHBufferUAV> D3D11Factory::CreateStructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
{
	return shared_ptr<RHBufferUAV>( new D3D11StructuredBufferUAV(buffer, elementCount) );
}

shared_ptr<RHBufferUAV> D3D11Factory::CreateTextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<RHBufferUAV>( new D3D11TextureBufferUAV(buffer, elementCount, format) );
}

shared_ptr<RHRenderView> D3D11Factory::CreateDepthStencilView( const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return shared_ptr<RHRenderView>( new D3D11DepthStencilView(texture, arrayIndex, level) );
}

shared_ptr<RHRenderView> D3D11Factory::CreateRenderTargetView2D( const shared_ptr<RHTexture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return shared_ptr<RHRenderView>( new D3D11RenderTargetView2D(texture, arrayIndex, level) );
}

shared_ptr<RHRenderView> D3D11Factory::CreateRenderTargetViewArray( const shared_ptr<RHTexture>& texture, uint32_t level )
{
	return shared_ptr<RHRenderView>( new D3D11RenderTargetViewArray(texture, level) );
}

shared_ptr<RHBlendState> D3D11Factory::CreateBlendStateImpl( const RHBlendStateDesc& desc )
{
	return shared_ptr<RHBlendState>( new D3D11BlendState(desc) );
}

shared_ptr<RHSamplerState> D3D11Factory::CreateSamplerStateImpl( const RHSamplerStateDesc& desc )
{
	return shared_ptr<RHSamplerState>( new D3D11SamplerState(desc) );
}

shared_ptr<RHRasterizerState> D3D11Factory::CreateRasterizerStateImpl( const RHRasterizerStateDesc& desc )
{
	return shared_ptr<RHRasterizerState>( new D3D11RasterizerState(desc) );
}

shared_ptr<RHDepthStencilState> D3D11Factory::CreateDepthStencilStateImpl( const RHDepthStencilStateDesc& desc )
{
	return shared_ptr<RHDepthStencilState>( new D3D11DepthStencilState(desc) );
}




}
