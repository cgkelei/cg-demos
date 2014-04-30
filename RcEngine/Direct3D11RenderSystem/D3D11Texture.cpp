#include "D3D11Texture.h"
#include <Core/Exception.h>

namespace RcEngine {

D3D11Texture::D3D11Texture( TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint, flags)
{
	mTextureArraySize = (std::max)(arraySize, 1U);
}


void D3D11Texture::BuildMipMap()
{

}

void D3D11Texture::Map1D(uint32_t arrayIndex,  uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map1D");
}

void D3D11Texture::Map2D(uint32_t arrayIndex,  uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map2D");
}

void D3D11Texture::Map3D(uint32_t arrayIndex,  uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Map3D");
}

void D3D11Texture::MapCube(uint32_t arrayIndex,  CubeMapFace face, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::MapCube");
}

void D3D11Texture::Unmap1D(uint32_t arrayIndex,   uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap1D");
}

void D3D11Texture::Unmap2D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap2D");
}

void D3D11Texture::Unmap3D(uint32_t arrayIndex,  uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::Unmap3D");
}

void D3D11Texture::UnmapCube(uint32_t arrayIndex,   CubeMapFace face, uint32_t level )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::UnmapCube");
}

void D3D11Texture::CopyToTexture( Texture& destTexture )
{
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Shoudn't be here!", "OpenGLTexture::CopyToTexture");
}







}