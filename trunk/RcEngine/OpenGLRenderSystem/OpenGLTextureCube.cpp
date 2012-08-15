#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

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
			glTexParameteri(mTargetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(mTargetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

			for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for (uint32_t level = 0; level < mMipMaps; ++ level)
				{
					for(uint32_t face = 0; face < 6; ++ face)
					{
						uint32_t levelSize= mSizes[level];
						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
								"OpenGLTexture1D::OpenGLTexture1D");
						}
						else
						{
							uint32_t imageSize = levelSize * levelSize * texelSize;
							uint32_t imageIndex =  arrIndex*mMipMaps*6 + face*mMipMaps + level;
							mTextureData[imageIndex].resize(imageSize);


							if(mTextureArraySize > 1)
							{
								ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported CubeMap Texture Array",
									"OpenGLTextureCube::OpenGLTextureCube");
							}
							else
							{
								glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, glinternalFormat, size, size, 0, glformat, gltype,
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
}

