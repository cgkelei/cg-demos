#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace RenderSystem
	{
		using Core::Exception;

		OpenGLTexture3D::OpenGLTexture3D( PixelFormat format, unsigned int arraySize, unsigned int numMipMaps, unsigned int width, unsigned int height, unsigned int depth, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint, ElementInitData* initData )
			: OpenGLTexture(TT_Texture3D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
		{
			// No 3D Texture Array 
			assert(arraySize == 1);

			if( numMipMaps == 0 )
			{
				mMipMaps = 1;
				unsigned int w = width;
				unsigned int h = height;
				unsigned int d = depth;
				while ((w > 1) && (h > 1) && (d > 1))
				{
					++mMipMaps;
					w = std::max<unsigned int>(1U, w / 2);
					h = std::max<unsigned int>(1U, h / 2);
					d = std::max<unsigned int>(1U, d / 2);
				}
			}
			else
			{
				mMipMaps = numMipMaps;
			}

			mWidths.resize(mMipMaps);
			mHeights.resize(mMipMaps);
			mDepths.resize(mMipMaps);
			{
				unsigned int w = width;
				unsigned int h = height;
				unsigned int d = depth;
				for(unsigned int level = 0; level < mMipMaps; level++)
				{
					mWidths[level] = w;
					mHeights[level] = h;
					mDepths[level] = d;
					w = std::max<unsigned int>(1U, w / 2);
					h = std::max<unsigned int>(1U, h / 2);
					d = std::max<unsigned int>(1U, d / 2);
				}
			}

			unsigned int texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

			GLint glinternalFormat;
			GLenum glformat;
			GLenum gltype;
			OpenGLMapping::Mapping(glinternalFormat, glformat, gltype, mFormat);

			mTextureData.resize(mMipMaps);

			glGenTextures(1, &mTextureID);
			glBindTexture(mTargetType, mTextureID);
			glTexParameteri(mTargetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(mTargetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

			for (unsigned int level = 0; level < mMipMaps; ++ level)
			{
				unsigned int levelWidth = mWidths[level];
				unsigned int levelHeight = mHeights[level];
				unsigned int levelDepth = mDepths[level];

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
						"OpenGLTexture1D::OpenGLTexture1D");
				}
				else
				{
					unsigned int imageSize = levelWidth * levelHeight * levelDepth * texelSize;
					mTextureData[level].resize(imageSize);
					glTexImage3D(mTargetType, level, glinternalFormat, levelWidth, levelHeight, levelDepth, 0,
						glformat, gltype, (NULL == initData) ? NULL : initData[level].pData);

				}
			}
		}

		OpenGLTexture3D::~OpenGLTexture3D()
		{

		};

		unsigned int OpenGLTexture3D::GetWidth( unsigned int level )
		{
			assert(level < mMipMaps);
			return mWidths[level];
		}

		unsigned int OpenGLTexture3D::GetHeight( unsigned int level )
		{
			assert(level < mMipMaps);
			return mHeights[level];
		}

		unsigned int OpenGLTexture3D::GetDepth( unsigned int level )
		{
			assert(level < mMipMaps);
			return mDepths[level];
		}

		void OpenGLTexture3D::Map3D( unsigned int arrayIndex, unsigned int level, TextureMapAccess tma, unsigned int xOffset, unsigned int yOffset, unsigned int zOffset, unsigned int width, unsigned int height, unsigned int depth, void*& data, unsigned int& rowPitch, unsigned int& slicePitch )
		{

		}

		void OpenGLTexture3D::Unmap3D( unsigned int arrayIndex, unsigned int level )
		{

		}

	}
}

