#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture3D::OpenGLTexture3D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t depth, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
	: OpenGLTexture(TT_Texture3D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	// No 3D Texture Array 
	assert(arraySize == 1);

	if( numMipMaps == 0 )
	{
		mMipMaps = 1;
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;
		while ((w > 1) && (h > 1) && (d > 1))
		{
			++mMipMaps;
			w = std::max<uint32_t>(1U, w / 2);
			h = std::max<uint32_t>(1U, h / 2);
			d = std::max<uint32_t>(1U, d / 2);
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
		uint32_t w = width;
		uint32_t h = height;
		uint32_t d = depth;
		for(uint32_t level = 0; level < mMipMaps; level++)
		{
			mWidths[level] = w;
			mHeights[level] = h;
			mDepths[level] = d;
			w = std::max<uint32_t>(1U, w / 2);
			h = std::max<uint32_t>(1U, h / 2);
			d = std::max<uint32_t>(1U, d / 2);
		}
	}

	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

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

	for (uint32_t level = 0; level < mMipMaps; ++ level)
	{
		uint32_t levelWidth = mWidths[level];
		uint32_t levelHeight = mHeights[level];
		uint32_t levelDepth = mDepths[level];

		if (PixelFormatUtils::IsCompressed(mFormat))
		{
			ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Currently Unsupported Compressed Texture Format",
				"OpenGLTexture1D::OpenGLTexture1D");
		}
		else
		{
			uint32_t imageSize = levelWidth * levelHeight * levelDepth * texelSize;
			mTextureData[level].resize(imageSize);
			glTexImage3D(mTargetType, level, glinternalFormat, levelWidth, levelHeight, levelDepth, 0,
				glformat, gltype, (NULL == initData) ? NULL : initData[level].pData);

		}
	}
}

OpenGLTexture3D::~OpenGLTexture3D()
{

};

uint32_t OpenGLTexture3D::GetWidth( uint32_t level )
{
	assert(level < mMipMaps);
	return mWidths[level];
}

uint32_t OpenGLTexture3D::GetHeight( uint32_t level )
{
	assert(level < mMipMaps);
	return mHeights[level];
}

uint32_t OpenGLTexture3D::GetDepth( uint32_t level )
{
	assert(level < mMipMaps);
	return mDepths[level];
}

void OpenGLTexture3D::Map3D( uint32_t arrayIndex, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
{

}

void OpenGLTexture3D::Unmap3D( uint32_t arrayIndex, uint32_t level )
{

}

}

