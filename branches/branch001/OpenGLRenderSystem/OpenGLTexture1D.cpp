#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;

		OpenGLTexture1D::OpenGLTexture1D( PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
			: OpenGLTexture(TT_Texture1D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			if( numMipMaps == 0 )
			{
				mMipMaps = 1;
				unsigned int w = width;
				while( w!= 1)
				{
					++mMipMaps;
					w = std::max<unsigned int>(1U, w / 2);
				}
			}
			else
			{
				mMipMaps = numMipMaps;
			}

			mWidths.resize(mMipMaps);
			{
				unsigned int w = width;
				for(unsigned int level = 0; level < mMipMaps; level++)
				{
					mWidths[level] = w;
					w = std::max<unsigned int>(1U, w / 2);
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

				for (unsigned int arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
				{
					for (unsigned int level = 0; level < mMipMaps; ++ level)
					{
						unsigned int levelWidth = mWidths[level];

						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
								"OpenGLTexture1D::OpenGLTexture1D");
						}
						else
						{
							GLsizei imageSize = level * texelSize;
							mTextureData[arrIndex * mMipMaps + level].resize(imageSize);

							if (mTextureArraySize > 1)
							{
								if (0 == arrIndex)
								{
									glTexImage2D(mTargetType, level, glinternalFormat, levelWidth, mTextureArraySize, 0, glformat, gltype, NULL);
								}

								glTexSubImage2D(mTargetType, level, 0, arrIndex, levelWidth, 1,
									glformat, gltype, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
							}
							else
							{
								glTexImage1D(mTargetType, level, glinternalFormat, levelWidth, 0, glformat, gltype,
									(NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
							}

						}
					}
				}
			}
		}

		OpenGLTexture1D::~OpenGLTexture1D()
		{

		}


		unsigned int OpenGLTexture1D::GetWidth( unsigned int level )
		{
			assert(level < mMipMaps);
			return mWidths[level];
		}



		void OpenGLTexture1D::Map1D( unsigned int arrayIndex, unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int width, void*& data )
		{

		}

		void OpenGLTexture1D::Unmap1D( unsigned int arrayIndex, unsigned int level )
		{

		}

	}
}

