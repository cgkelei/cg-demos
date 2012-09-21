#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTextureCube::OpenGLTextureCube( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
	: OpenGLTexture(TT_TextureCube, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	assert(height == width);
	uint32_t size = width;

	if( numMipMaps == 0 )
	{
		mMipMaps = 1;
		uint32_t w = size;
		while( w > 1)
		{
			++mMipMaps;
			w = std::max<uint32_t>(1U, w / 2);
		}
	}
	else
	{
		mMipMaps = numMipMaps;
	}

	mSizes.resize(mMipMaps);
	{
		uint32_t w = size;
		for(uint32_t level = 0; level < mMipMaps; level++)
		{
			mSizes[level] = w;
			w = std::max<uint32_t>(1U, w / 2);
		}
	}

	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	GLint glinternalFormat;
	GLenum glformat;
	GLenum gltype;
	OpenGLMapping::Mapping(glinternalFormat, glformat, gltype, mFormat);

	mTextureData.resize(mTextureArraySize * mMipMaps * 6);


	glGenTextures(1, &mTextureID);
	glBindTexture(mTargetType, mTextureID);
	glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++arrIndex)
	{
		for(uint32_t face = 0; face < 6; ++face)
		{
			for (uint32_t level = 0; level < mMipMaps; ++level)
			{
				uint32_t levelSize= mSizes[level];

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (glinternalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelSize+3)/4)*((levelSize+3)/4)*blockSize; 

					uint32_t imageIndex =  arrIndex*mMipMaps*6 + face*mMipMaps + level;

					if (!mPixelBuffers.empty())
					{
						glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
					}
					else
					{
						// resize texture data for copy
						mTextureData[imageIndex].resize(imageSize);
					}

					if (mTextureArraySize > 1)
					{	
						if (0 == arrIndex)
						{
							glCompressedTexImage3D(mTargetType, level, glinternalFormat, levelSize, levelSize, mTextureArraySize,
								0, imageSize, NULL);
						}
						glCompressedTexSubImage3D(mTargetType, level, 0, 0, arrIndex, levelSize, levelSize, 1, glinternalFormat, 
							imageSize, (NULL == initData) ? NULL : initData[imageIndex].pData);

					}
					else
					{
						
						glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, glinternalFormat, levelSize, levelSize, 0,
							imageSize, (NULL == initData) ? NULL : initData[imageIndex].pData);
					}


				}
				else
				{
					uint32_t imageSize = levelSize * levelSize * texelSize;

					uint32_t imageIndex =  arrIndex * mMipMaps * 6 + face * mMipMaps + level;
					mTextureData[imageIndex].resize(imageSize);

					if(mTextureArraySize > 1)
					{
						ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported CubeMap Texture Array",
							"OpenGLTextureCube::OpenGLTextureCube");
					}
					else
					{
						ElementInitData id = initData[imageIndex];
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, glinternalFormat, levelSize, levelSize, 0, glformat, gltype,
							(NULL == initData) ? NULL : initData[imageIndex].pData);
					}
				}	
			}
		}
	}
}


OpenGLTextureCube::~OpenGLTextureCube()
{

}


uint32_t OpenGLTextureCube::GetWidth( uint32_t level )
{
	assert(level < mMipMaps);
	return mSizes[level];
}


uint32_t OpenGLTextureCube::GetHeight( uint32_t level )
{
	assert(level < mMipMaps);
	return mSizes[level];
}


void OpenGLTextureCube::MapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{

}


void OpenGLTextureCube::UnmapCube( uint32_t arrayIndex, CubeMapFace face, uint32_t level )
{

}

}

