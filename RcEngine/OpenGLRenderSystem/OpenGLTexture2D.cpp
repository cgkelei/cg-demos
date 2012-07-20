#include "OpenGLTexture.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
			: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			if( numMipMaps == 0 )
			{
				mMipMaps = 1;
				uint32_t w = width;
				uint32_t h = height;
				while( w!= 1 || h != 1)
				{
					++mMipMaps;
					w = std::max<uint32_t>(1U, w / 2);
					h = std::max<uint32_t>(1U, h / 2);
				}
			}
			else
			{
				mMipMaps = numMipMaps;
			}

			mWidths.resize(mMipMaps);
			mHeights.resize(mMipMaps);
			{
				uint32_t w = width;
				uint32_t h = height;
				for(uint32_t level = 0; level < mMipMaps; level++)
				{
					mWidths[level] = w;
					mHeights[level] = h;
					w = std::max<uint32_t>(1U, w / 2);
					h = std::max<uint32_t>(1U, h / 2);
				}
			}

			uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

			GLint glinternalFormat;
			GLenum glformat;
			GLenum gltype;
			OpenGLMapping::Mapping(glinternalFormat, glformat, gltype, mFormat);

			mTextureData.resize(mTextureArraySize * mMipMaps);


			if(mSampleCount <= 1)
			{
				glGenTextures(1, &mTextureID);
				glBindTexture(mTargetType, mTextureID);
				glTexParameteri(mTargetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(mTargetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

				for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
				{
					for (uint32_t level = 0; level < mMipMaps; ++ level)
					{
						uint32_t levelWidth = mWidths[level];
						uint32_t levelHeight = mHeights[level];

						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
								"OpenGLTexture1D::OpenGLTexture1D");
						}
						else
						{
							uint32_t imageSize = levelWidth * levelHeight * texelSize;
							mTextureData[arrIndex * mMipMaps + level].resize(imageSize);

							if (mTextureArraySize > 1)
							{
								if (0 == arrIndex)
								{
									glTexImage3D(mTargetType, level, glinternalFormat, levelWidth, 
										levelHeight, mTextureArraySize, 0, glformat, gltype, NULL);
								}

								glTexSubImage3D(mTargetType, level, 0, 0, arrIndex, levelWidth, levelHeight, 1,
									glformat, gltype, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
							}
							else
							{

								glTexImage2D(mTargetType, level, glinternalFormat, levelWidth, levelHeight, 0, glformat, gltype,
									(NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
							}

						}
					}
				}
			}
			else
			{
				// Multisample, For render target
				glGenRenderbuffersEXT(1, &mTextureID);
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mTextureID);
				glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, mSampleCount, glinternalFormat, width, height);
			}
		}

		OpenGLTexture2D::~OpenGLTexture2D()
		{

		}

		uint32_t OpenGLTexture2D::GetWidth( uint32_t level )
		{
			assert(level < mMipMaps);
			return mWidths[level];
		}

		uint32_t OpenGLTexture2D::GetHeight( uint32_t level )
		{
			assert(level < mMipMaps);
			return mHeights[level];
		}

		void OpenGLTexture2D::Map2D( uint32_t arrayIndex, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
		{

		}


		void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
		{

		}

	}
}

