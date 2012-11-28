#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
	: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	if( numMipMaps == 0 )
	{
		// calculate mip map levels
		mMipMaps = 1;
		/*uint32_t w = width;
		uint32_t h = height;
		while( w!= 1 || h != 1)
		{
			++mMipMaps;
			w = std::max<uint32_t>(1U, w / 2);
			h = std::max<uint32_t>(1U, h / 2);
		}*/
	}
	else
	{
		mMipMaps = numMipMaps;
	}

	mWidths.resize(mMipMaps);
	mHeights.resize(mMipMaps);
	{
		// store every level width and height
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

	if (GLEW_ARB_pixel_buffer_object)
	{
		mPixelBuffers.resize(mTextureArraySize*mMipMaps);
		glGenBuffers(static_cast<GLsizei>(mPixelBuffers.size()), &mPixelBuffers[0]);
	}
	else
	{
		mTextureData.resize(mTextureArraySize * mMipMaps);
	}
	
	if(mSampleCount <= 1)
	{
		// not multiple sample 
		glGenTextures(1, &mTextureID);
		glBindTexture(mTargetType, mTextureID);
		glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

		for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
		{
			for (uint32_t level = 0; level < mMipMaps; ++ level)
			{
				uint32_t levelWidth = mWidths[level];
				uint32_t levelHeight = mHeights[level];

				if (!mPixelBuffers.empty())
				{
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBuffers[arrIndex * mMipMaps + level]);
				}

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (glinternalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
					
					if (!mPixelBuffers.empty())
					{
						glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
					}
					else
					{
						// resize texture data for copy
						mTextureData[arrIndex * mMipMaps + level].resize(imageSize);
					}

					if (mTextureArraySize > 1)
					{	
						if (0 == arrIndex)
						{
							glCompressedTexImage3D(mTargetType, level, glinternalFormat, levelWidth, levelHeight, mTextureArraySize,
								0, imageSize, NULL);
						}
						glCompressedTexSubImage3D(mTargetType, level, 0, 0, arrIndex, levelWidth, levelHeight, 1, glinternalFormat, 
							imageSize, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
								
					}
					else
					{
						glCompressedTexImage2D(GL_TEXTURE_2D, level, glinternalFormat, levelWidth, levelHeight, 0,
							imageSize, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
					}
				}
				else
				{
					uint32_t imageSize = levelWidth * levelHeight * texelSize;

					if (!mPixelBuffers.empty())
					{
						glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
						glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
					}
					else
					{
						// resize texture data for copy
						mTextureData[arrIndex * mMipMaps + level].resize(imageSize);
					}				

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
	// store 
	mTextureMapAccess = tma;

	GLint glinternalFormat;
	GLenum glformat;
	GLenum gltype;
	OpenGLMapping::Mapping(glinternalFormat, glformat, gltype, mFormat);

	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	int blockSize = 0;
	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		blockSize = (glinternalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	}

	rowPitch = mWidths[level] * texelSize; 

	uint8_t* p;
	switch(tma)
	{
	case TMA_Read_Only:
	case TMA_Read_Write:
		{
			if (!mPixelBuffers.empty())
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBuffers[arrayIndex * mMipMaps + level]);

				glBindTexture(mTargetType, mTextureID);
				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					glGetCompressedTexImage(mTargetType, level, NULL);
					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
				}
				else
				{
					glGetTexImage(mTargetType, level, glformat, gltype, NULL);
					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
				}
			}
			else
			{
				glBindTexture(mTargetType, mTextureID);
				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					glGetCompressedTexImage(mTargetType, level, &mTextureData[arrayIndex * mMipMaps + level][0]);
					p = &mTextureData[arrayIndex * mMipMaps + level][0];
				}
				else
				{
					glGetTexImage(mTargetType, level, glformat, gltype, &mTextureData[arrayIndex * mMipMaps + level][0]);
					p = &mTextureData[arrayIndex * mMipMaps + level][0];
				}
			}
		}
		break;

	case TMA_Write_Only:
		{
			if (!mPixelBuffers.empty())
			{
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBuffers[arrayIndex * mMipMaps + level]);
				p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY));
			}
			else
			{
				p = &mTextureData[arrayIndex * mMipMaps + level][0];
			}
		}
		break;

	default:
		assert(false);
	}

	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		data = p + (yOffset / 4) * rowPitch + (xOffset / 4 * blockSize);
	}
	else
	{
		data = p + (yOffset * mWidths[level] + xOffset) * texelSize;
	}

}

void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
{
	switch(mTextureMapAccess)
	{
	case TMA_Read_Only:
		{
			if (!mPixelBuffers.empty())
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBuffers[arrayIndex * mMipMaps + level]);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
			}
			break;
		}

	case TMA_Write_Only:
	case TMA_Read_Write:
		{
			GLint glinternalFormat;
			GLenum glformat;
			GLenum gltype;
			OpenGLMapping::Mapping(glinternalFormat, glformat, gltype, mFormat);
			
			uint32_t imageSize = 0;
			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				int blockSize = (glinternalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				imageSize = ((mWidths[level]+3)/4)*((mHeights[level]+3)/4)*blockSize; 
			}

			glBindTexture(mTargetType, mTextureID);

			uint8_t* p;
			if (!mPixelBuffers.empty())
			{

				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBuffers[arrayIndex * mMipMaps + level]);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

				p = NULL;
			}
			else
			{
				// resize texture data for copy
				p = &mTextureData[arrayIndex * mMipMaps + level][0];
			}

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				if (mTextureArraySize > 1)
				{	
					glCompressedTexSubImage3D(mTargetType, level, 0, 0, arrayIndex, mWidths[level], mHeights[level], 1, glformat, imageSize, p);
				}
				else
				{
					glCompressedTexSubImage2D(mTargetType, level, 0, 0, mWidths[level], mHeights[level], glformat, imageSize, p);
				}
			}
			else
			{
				if (mTextureArraySize > 1)
				{	
					glTexSubImage3D(mTargetType, level, 0, 0, arrayIndex, mWidths[level], mHeights[level], 1, glformat, gltype, p);
				}
				else
				{
					glTexSubImage2D(mTargetType, level, 0, 0, mWidths[level], mHeights[level],  glformat, gltype, p);
				}

			}
		}
	}
}

}


