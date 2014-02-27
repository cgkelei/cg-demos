#include "OpenGLTexture.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData )
	: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint)
{
	// numMipMap == 0, will generate mipmap levels automatically
	if( numMipMaps == 0 )
	{
		// Calculate full mipmap levels
		mMipMaps = 1;
		while ((width|height) >> mMipMaps) mMipMaps++;
	}
	else
	{
		mMipMaps = numMipMaps;
	}

	// keep every level width and height
	mWidths.resize(mMipMaps);
	mHeights.resize(mMipMaps);
	{		
		for(uint32_t level = 0; level < mMipMaps; level++)
		{
			mWidths[level] = std::max<uint32_t>(1U, width >> level);
			mHeights[level] = std::max<uint32_t>(1U, height >> level);
		}
	} 

	// Only CPU side access can use Map
	bool cpuSideAccess = (accessHint & (EAH_CPU_Read | EAH_CPU_Write)) != 0;
	if ( cpuSideAccess )
	{
		if (GLEW_ARB_pixel_buffer_object)
			glGenBuffers(1, &mPixelBuffer);
		else
			mTextureData.resize(mTextureArraySize * mMipMaps);
	}
	
	if(mSampleCount <= 1)
	{
		// not multiple sample 
		glGenTextures(1, &mTextureID);
		glBindTexture(mTargetType, mTextureID);
		glTexParameteri(mTargetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(mTargetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(mTargetType, GL_TEXTURE_MAX_LEVEL, mMipMaps - 1);

		// Use texture storage to init, faster
		if (GLEW_ARB_texture_storage)
			CreateWithImmutableStorage(initData);
		else
			CreateWithMutableStorage(initData);
	}
	else
	{
		GLenum internalFormat, externFormat, formatType;
		OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);

		// Multisample, For render target
		glGenRenderbuffers(1, &mTextureID);
		glBindRenderbuffer(GL_RENDERBUFFER, mTextureID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSampleCount, internalFormat, width, height);
	}
}

OpenGLTexture2D::~OpenGLTexture2D()
{

}

void OpenGLTexture2D::CreateWithImmutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	if (mTextureArraySize > 1)
	{
		glTexStorage3D(mTargetType, mMipMaps, internalFormat, mWidths[0], mHeights[0], mTextureArraySize);
		if (initData)
		{
			for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for (uint32_t level = 0; level < mMipMaps; ++ level)
				{
					uint32_t levelWidth = mWidths[level];
					uint32_t levelHeight = mHeights[level];
					if (PixelFormatUtils::IsCompressed(mFormat))
					{
						int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
						uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
						glCompressedTexSubImage3D(mTargetType,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(0),
							externFormat, 
							static_cast<GLsizei>(imageSize),
							initData[arrIndex * mMipMaps + level].pData);
					}
					else
					{
						glTexSubImage3D(mTargetType,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(1),
							externFormat,
							formatType,
							initData[arrIndex * mMipMaps + level].pData);
					}
				}
			}
		}
	}
	else
	{
		glTexStorage2D(mTargetType, mMipMaps, internalFormat, mWidths[0], mHeights[0]);
		if (initData)
		{
			for (uint32_t level = 0; level < mMipMaps; ++ level)
			{
				uint32_t levelWidth = mWidths[level];
				uint32_t levelHeight = mHeights[level];
				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

					glCompressedTexSubImage2D(mTargetType,
						static_cast<GLint>(level), 0, 0,
						static_cast<GLsizei>(levelWidth),
						static_cast<GLsizei>(levelHeight),
						externFormat,
						static_cast<GLsizei>(imageSize),
						initData[level].pData);
				}
				else
				{
					glTexSubImage2D(mTargetType,
						static_cast<GLint>(level), 0, 0,
						static_cast<GLsizei>(levelWidth),
						static_cast<GLsizei>(levelHeight),
						externFormat,
						formatType,
						initData[level].pData);
				}
			}
		}
	}
}

void OpenGLTexture2D::CreateWithMutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
	{
		for (uint32_t level = 0; level < mMipMaps; ++ level)
		{
			uint32_t levelWidth = mWidths[level];
			uint32_t levelHeight = mHeights[level];

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				// Need check
				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

				if (mTextureArraySize > 1)
				{	
					if (0 == arrIndex)
					{
						glCompressedTexImage3D(mTargetType, level, internalFormat, levelWidth, levelHeight, 
							mTextureArraySize, 0, imageSize, NULL);
					}

					glCompressedTexSubImage3D(mTargetType, level, 0, 0, arrIndex, levelWidth, levelHeight, 1, internalFormat, 
							imageSize, initData[arrIndex * mMipMaps + level].pData);
				}
				else
				{
					glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, levelWidth, levelHeight, 0,
						imageSize, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
				}
			}
			else
			{
				if (mTextureArraySize > 1)
				{
					if (0 == arrIndex)
					{
						glTexImage3D(mTargetType, level, internalFormat, levelWidth, levelHeight,
							mTextureArraySize, 0, externFormat, formatType, NULL);
					}

					// OpenGL bugs. init texture array mipmaps with NULL storage cause crash.
					if (initData)
					{
						glTexSubImage3D(mTargetType, level, 0, 0, arrIndex, levelWidth, levelHeight, 1,
							externFormat, formatType, (NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
					}
					else
					{
						if (mMipMaps > 1)
							glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
					}
					
				}
				else
				{
					glTexImage2D(mTargetType, level, internalFormat, levelWidth, levelHeight, 0, externFormat, formatType,
						(NULL == initData) ? NULL : initData[arrIndex * mMipMaps + level].pData);
				}
			}
		}
	}
}

uint32_t OpenGLTexture2D::GetWidth( uint32_t level ) const
{
	assert(level < mMipMaps);
	return mWidths[level];
}

uint32_t OpenGLTexture2D::GetHeight( uint32_t level ) const
{
	assert(level < mMipMaps);
	return mHeights[level];
}

void OpenGLTexture2D::Map2D( uint32_t arrayIndex, uint32_t level, TextureMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
	if ( (mAccessHint & (EAH_CPU_Read | EAH_CPU_Write)) == 0 )
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Map only work with CPU side access!", "OpenGLTexture2D::Map2D");

	mTextureMapAccess = tma;

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	rowPitch = mWidths[level] * texelSize; 

	// Compute image size
	uint32_t imageSize = mWidths[level] * mHeights[level] * texelSize;
	int blockSize = 0;
	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
		imageSize = ((width+3)/4)*((height+3)/4)*blockSize; 
	}

	uint8_t* p;
	switch(tma)
	{
	case TMA_Read_Only:
	case TMA_Read_Write:
		{
			if (mPixelBuffer != 0) 
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBuffer);
				glBufferData(GL_PIXEL_PACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);

				glBindTexture(mTargetType, mTextureID);

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					glGetCompressedTexImage(mTargetType, level, NULL);
					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
				}
				else
				{
					glGetTexImage(mTargetType, level, externFormat, formatType, NULL);
					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
				}
			}
			else
			{
				mTextureData.resize(imageSize);
				glBindTexture(mTargetType, mTextureID);
				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					glGetCompressedTexImage(mTargetType, level, &mTextureData[0]);
					p = &mTextureData[0];
				}
				else
				{
					glGetTexImage(mTargetType, level, externFormat, formatType, &mTextureData[0]);
					p = &mTextureData[0];
				}
			}
		}
		break;

	case TMA_Write_Only:
		{
			if (mPixelBuffer != 0) 
			{
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBuffer);
				glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
				p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY));
			}
			else
			{
				mTextureData.resize(imageSize);
				p = &mTextureData[0];
			}
		}
		break;

	default:
		assert(false);
	}

	if (PixelFormatUtils::IsCompressed(mFormat))
		data = p + (yOffset / 4) * rowPitch + (xOffset / 4 * blockSize);
	else
		data = p + (yOffset * mWidths[level] + xOffset) * texelSize;
}

void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
{
	if ( (mAccessHint & (EAH_CPU_Read | EAH_CPU_Write)) == 0 )
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Map only work with CPU side access!", "OpenGLTexture2D::Unmap2D");

	switch(mTextureMapAccess)
	{
	case TMA_Read_Only:
		{
			if (mPixelBuffer != 0) 
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBuffer);
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
			}
			break;
		}

	case TMA_Write_Only:
	case TMA_Read_Write:
		{
			GLenum internalFormat, externFormat, formatType;
			OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
			uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);
			
			uint32_t imageSize = 0;
			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				imageSize = ((mWidths[level]+3)/4)*((mHeights[level]+3)/4)*blockSize; 
			}

			glBindTexture(mTargetType, mTextureID);

			uint8_t* p;
			if (mPixelBuffer != 0) 
			{
				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBuffer);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
				p = NULL;
			}
			else
				p = &mTextureData[0];

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				if (mTextureArraySize > 1)
					glCompressedTexSubImage3D(mTargetType, level, 0, 0, arrayIndex, mWidths[level], mHeights[level], 1, externFormat, imageSize, p);
				else
					glCompressedTexSubImage2D(mTargetType, level, 0, 0, mWidths[level], mHeights[level], externFormat, imageSize, p);
			}
			else
			{
				if (mTextureArraySize > 1)
					glTexSubImage3D(mTargetType, level, 0, 0, arrayIndex, mWidths[level], mHeights[level], 1, externFormat, formatType, p);
				else
					glTexSubImage2D(mTargetType, level, 0, 0, mWidths[level], mHeights[level],  externFormat, formatType, p);
			}
		}
	}
}

}


