#include "D3D11Texture.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11TextureBase::D3D11TextureBase( TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint),
	  mFlags(flags),
	  mShaderResourceView(nullptr)
{

}


void D3D11TextureBase::BuildMipMap()
{

}

void D3D11TextureBase::Map1D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map1D");
}

void D3D11TextureBase::Map2D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map2D");
}

void D3D11TextureBase::Map3D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map3D");
}

void D3D11TextureBase::MapCube(uint32_t arrayIndex,  CubeMapFace face, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::MapCube");
}

void D3D11TextureBase::Unmap1D(uint32_t arrayIndex,   uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap1D");
}

void D3D11TextureBase::Unmap2D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap2D");
}

void D3D11TextureBase::Unmap3D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap3D");
}

void D3D11TextureBase::UnmapCube(uint32_t arrayIndex,   CubeMapFace face, uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::UnmapCube");
}

void D3D11TextureBase::CopyToTexture( Texture& destTexture )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::CopyToTexture");
}




}