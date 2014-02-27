#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture1D::OpenGLTexture1D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
	: OpenGLTexture(TT_Texture1D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	if( numMipMaps == 0 )
	{
		mMipMaps = 1;
		uint32_t w = width;
		while( w!= 1)
		{
			++mMipMaps;
			w = std::max<uint32_t>(1U, w / 2);
		}
	}
	else
	{
		mMipMaps = numMipMaps;
	}

	mWidths.resize(mMipMaps);
	{
		uint32_t w = width;
		for(uint32_t level = 0; level < mMipMaps; level++)
		{
			mWidths[level] = w;
			w = std::max<uint32_t>(1U, w / 2);
		}
	}

	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);

	if(mSampleCount <= 1)
	{
		glGenTextures(1, &mTextureID);
		glBindTexture(mTargetType, mTextureID);
		glTexParameteri(mTargetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(mTargetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

		for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
		{
			for (uint32_t level = 0; level < mMipMaps; ++ level)
			{
				uint32_t levelWidth = mWidths[level];

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
						"OpenGLTexture1D::OpenGLTexture1D");
				}
				else
				{
					if (mTextureArraySize > 1)
					{
						if (0 == arrIndex)
						{
							glTexImage2D(mTargetType, level, internalFormat, levelWidth, mTextureArraySize, 0, externFormat, formatType, NULL);
						}

						glTexSubImage2D(mTargetType, level, 0, arrIndex, levelWidth, 1,
							externFormat, formatType, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
					}
					else
					{
						glTexImage1D(mTargetType, level, internalFormat, levelWidth, 0, externFormat, formatType,
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


uint32_t OpenGLTexture1D::GetWidth( uint32_t level ) const
{
	assert(level < mMipMaps);
	return mWidths[level];
}



void OpenGLTexture1D::Map1D( uint32_t arrayIndex, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t width, void*& data )
{

}

void OpenGLTexture1D::Unmap1D( uint32_t arrayIndex, uint32_t level )
{

}

}

