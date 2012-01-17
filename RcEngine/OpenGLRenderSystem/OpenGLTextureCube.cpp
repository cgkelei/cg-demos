#include "OpenGLTexture.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		using Core::Exception;

		OpenGLTextureCube::OpenGLTextureCube( PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int height, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
			: OpenGLTexture(TT_TextureCube, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			assert(height == width);
			unsigned int size = width;

			if( numMipMaps == 0 )
			{
				mMipMaps = 1;
				unsigned int w = size;
				while( w > 1)
				{
					++mMipMaps;
					w = std::max<unsigned int>(1U, w / 2);
				}
			}
			else
			{
				mMipMaps = numMipMaps;
			}

			mSizes.resize(mMipMaps);
			{
				unsigned int w = size;
				for(unsigned int level = 0; level < mMipMaps; level++)
				{
					mSizes[level] = w;
					w = std::max<unsigned int>(1U, w / 2);
				}
			}

			unsigned int texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

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

			for (unsigned int  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for (unsigned int level = 0; level < mMipMaps; ++ level)
				{
					for(unsigned int face = 0; face < 6; ++ face)
					{
						unsigned int levelSize= mSizes[level];
						if (PixelFormatUtils::IsCompressed(mFormat))
						{
							ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
								"OpenGLTexture1D::OpenGLTexture1D");
						}
						else
						{
							unsigned int imageSize = levelSize * levelSize * texelSize;
							unsigned int imageIndex =  arrIndex*mMipMaps*6 + face*mMipMaps + level;
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


		unsigned int OpenGLTextureCube::GetWidth( unsigned int level )
		{
			assert(level < mMipMaps);
			return mSizes[level];
		}


		unsigned int OpenGLTextureCube::GetHeight( unsigned int level )
		{
			assert(level < mMipMaps);
			return mSizes[level];
		}


		void OpenGLTextureCube::MapCube( unsigned int arrayIndex, CubeMapFace face, unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height, void*& data, unsigned int& rowPitch )
		{

		}


		void OpenGLTextureCube::UnmapCube( unsigned int arrayIndex, CubeMapFace face, unsigned int level )
		{

		}

	}
}

