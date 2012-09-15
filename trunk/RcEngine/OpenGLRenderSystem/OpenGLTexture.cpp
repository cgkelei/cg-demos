#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture::OpenGLTexture( TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint )
	: Texture(type, format, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	mTextureArraySize = arraySize;

	if ( (mTextureArraySize > 1) && !GLEW_EXT_texture_array)
	{
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture Array is not supported!", "OpenGLTexture::OpenGLTexture");
	}

	switch(mType)
	{
	case TT_Texture1D:
		mTargetType =  (mTextureArraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
		break;
	case TT_Texture2D:
		mTargetType =  (mTextureArraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
		break;
	case  TT_Texture3D:
		mTargetType = GL_TEXTURE_3D;
		break;
	case  TT_TextureCube:
		mTargetType = (mTextureArraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
		break;
	default:
		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Texture type not supported!", "OpenGLTexture::OpenGLTexture");
	}
}


OpenGLTexture::~OpenGLTexture(void)
{
}

uint32_t OpenGLTexture::GetWidth( uint32_t level )
{
	assert(level < mMipMaps);
	return 1;
}

uint32_t OpenGLTexture::GetHeight( uint32_t level )
{
	assert(level < mMipMaps);
	return 1;
}

uint32_t OpenGLTexture::GetDepth( uint32_t level )
{
	assert(level < mMipMaps);
	return 1;
}

void OpenGLTexture::Map1D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{

}

void OpenGLTexture::Map2D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{

}

void OpenGLTexture::Map3D(uint32_t arrayIndex,  uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
{

}

void OpenGLTexture::MapCube(uint32_t arrayIndex,  CubeMapFace face, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{

}

void OpenGLTexture::Unmap1D(uint32_t arrayIndex,   uint32_t level )
{

}

void OpenGLTexture::Unmap2D(uint32_t arrayIndex,  uint32_t level )
{

}

void OpenGLTexture::Unmap3D(uint32_t arrayIndex,  uint32_t level )
{

}

void OpenGLTexture::UnmapCube(uint32_t arrayIndex,   CubeMapFace face, uint32_t level )
{

}

}





