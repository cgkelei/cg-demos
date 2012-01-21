#include "OpenGLTexture.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
			: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			if( numMipMaps == 0 )
			{
				mMipMaps = 1;
				unsigned int w = width;
				unsigned int h = height;
				while( w!= 1 || h != 1)
				{
					++mMipMaps;
					w = std::max<unsigned int>(1U, w / 2);
					h = std::max<unsigned int>(1U, h / 2);
				}
			}
			else
			{
				mMipMaps = numMipMaps;
			}

			mWidths.resize(mMipMaps);
			mHeights.resize(mMipMaps);
			{
				unsigned int w = width;
				unsigned int h = height;
				for(unsigned int level = 0; level < mMipMaps; level++)
				{
					mWidths[level] = w;
					mHeights[level] = h;
					w = std::max<unsigned int>(1U, w / 2);
					h = std::max<unsigned int>(1U, h / 2);
				}
			}

			unsigned int texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

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

				for (unsigned int  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
				{
					for (unsigned int level = 0; level < mMipMaps; ++ level)
					{
						unsigned int levelWidth = mWidths[level];
						unsigned int levelHeight = mHeights[level];

						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
								"OpenGLTexture1D::OpenGLTexture1D");
						}
						else
						{
							unsigned int imageSize = levelWidth * levelHeight * texelSize;
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

		unsigned int OpenGLTexture2D::GetWidth( unsigned int level )
		{
			assert(level < mMipMaps);
			return mWidths[level];
		}

		unsigned int OpenGLTexture2D::GetHeight( unsigned int level )
		{
			assert(level < mMipMaps);
			return mHeights[level];
		}

		void OpenGLTexture2D::Map2D( unsigned int arrayIndex, unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, void*& data, unsigned int& rowPitch )
		{

		}


		void OpenGLTexture2D::Unmap2D( unsigned int arrayIndex, unsigned int level )
		{

		}

	}
}


