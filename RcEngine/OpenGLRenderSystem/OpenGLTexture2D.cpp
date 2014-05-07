#include "OpenGLTexture.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLView.h"
#include <Core/Exception.h>

namespace RcEngine {

OpenGLTexture2D::OpenGLTexture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: OpenGLTexture(TT_Texture2D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags)
{
	// numMipMap == 0, will generate mipmap levels automatically
	mMipLevels = (numMipMaps > 0) ? numMipMaps : Texture::CalculateMipmapLevels((std::max)(width, height));
	mWidth = width;
	mHeight = height;

	// Only CPU side access can use Map
	bool cpuSideAccess = (accessHint & (EAH_CPU_Read | EAH_CPU_Write)) != 0;
	if ( cpuSideAccess && GLEW_ARB_pixel_buffer_object)
	{
		glGenBuffers(1, &mPixelBufferID);
	}

	// OpenGL Texture target type
	if (sampleCount <= 1)
		mTextureTarget = (mTextureArraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
	else
	{
		assert(mMipLevels <= 1);
		mTextureTarget =  (mTextureArraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
	}
	
	glGenTextures(1, &mTextureOGL);
	glBindTexture(mTextureTarget, mTextureOGL);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);

	// Use texture storage to init, faster
	if (GLEW_ARB_texture_storage)
		CreateWithImmutableStorage(initData);
	else
		CreateWithMutableStorage(initData);

	if (mCreateFlags & TexCreate_ShaderResource)
	{
		mTextureSRV = std::make_shared<OpenGLTextureSRV>(mTextureOGL, mTextureTarget);
	}
}

void OpenGLTexture2D::CreateWithImmutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	if (mTextureArraySize > 1)
	{
		glTexStorage3D(mTextureTarget, mMipLevels, internalFormat, mWidth, mHeight, mTextureArraySize);
		if (initData)
		{
			for (uint32_t  arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
			{
				for (uint32_t level = 0; level < mMipLevels; ++ level)
				{
					uint32_t levelWidth = (std::max)(1U, mWidth >> level);
					uint32_t levelHeight = (std::max)(1U, mHeight >> level);

					if (PixelFormatUtils::IsCompressed(mFormat))
					{
						int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
						uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
						
						glCompressedTexSubImage3D(mTextureTarget,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(0),
							externFormat, 
							static_cast<GLsizei>(imageSize),
							initData[arrIndex * mMipLevels + level].pData);
					}
					else
					{
						glTexSubImage3D(mTextureTarget,
							static_cast<GLint>(level),
							0, 0, static_cast<GLint>(arrIndex),
							static_cast<GLsizei>(levelWidth),
							static_cast<GLsizei>(levelHeight),
							static_cast<GLsizei>(1),
							externFormat,
							formatType,
							initData[arrIndex * mMipLevels + level].pData);
					}
				}
			}
		}
	}
	else
	{
		glTexStorage2D(mTextureTarget, mMipLevels, internalFormat, mWidth, mHeight);
		if (initData)
		{
			for (uint32_t level = 0; level < mMipLevels; ++ level)
			{
				uint32_t levelWidth = (std::max)(1U, mWidth >> level);
				uint32_t levelHeight = (std::max)(1U, mHeight >> level);

				if (PixelFormatUtils::IsCompressed(mFormat))
				{
					int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ? 8 : 16; 
					uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

					glCompressedTexSubImage2D(mTextureTarget,
						static_cast<GLint>(level), 0, 0,
						static_cast<GLsizei>(levelWidth),
						static_cast<GLsizei>(levelHeight),
						externFormat,
						static_cast<GLsizei>(imageSize),
						initData[level].pData);

					OGL_ERROR_CHECK();
				}
				else
				{
					glTexSubImage2D(mTextureTarget,
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

	OGL_ERROR_CHECK();
}

void OpenGLTexture2D::CreateWithMutableStorage(ElementInitData* initData)
{
	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++ arrIndex)
	{
		for (uint32_t level = 0; level < mMipLevels; ++ level)
		{
			uint32_t levelWidth = (std::max)(1U, mWidth >> level);
			uint32_t levelHeight = (std::max)(1U, mHeight >> level);

			if (PixelFormatUtils::IsCompressed(mFormat))
			{
				// Need check
				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
				uint32_t imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 

				if (mTextureArraySize > 1)
				{	
					if (0 == arrIndex)
					{
						glCompressedTexImage3D(mTextureTarget, level, internalFormat, levelWidth, levelHeight, 
							mTextureArraySize, 0, imageSize, NULL);
					}

					glCompressedTexSubImage3D(mTextureTarget, level, 0, 0, arrIndex, levelWidth, levelHeight, 1, internalFormat, 
							imageSize, initData[arrIndex * mMipLevels + level].pData);
				}
				else
				{
					glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, levelWidth, levelHeight, 0,
						imageSize, (NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
			}
			else
			{
				if (mTextureArraySize > 1)
				{
					if (0 == arrIndex)
					{
						glTexImage3D(mTextureTarget, level, internalFormat, levelWidth, levelHeight,
							mTextureArraySize, 0, externFormat, formatType, NULL);
					}

					// OpenGL bugs. init texture array mipmaps with NULL storage cause crash.
					if (initData)
					{
						glTexSubImage3D(mTextureTarget, level, 0, 0, arrIndex, levelWidth, levelHeight, 1,
							externFormat, formatType, (NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
					}
					else
					{
						if (mMipLevels > 1)
							glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
					}
					
				}
				else
				{
					glTexImage2D(mTextureTarget, level, internalFormat, levelWidth, levelHeight, 0, externFormat, formatType,
						(NULL == initData) ? NULL : initData[arrIndex * mMipLevels + level].pData);
				}
			}
		}
	}
}

void OpenGLTexture2D::Map2D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, void*& data, uint32_t& rowPitch )
{
//	if ( (mAccessHint & (EAH_CPU_Read | EAH_CPU_Write)) == 0 )
//		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Map only work with CPU side access!", "OpenGLTexture2D::Map2D");
//
//	mTextureMapAccess = tma;
//
//	GLenum internalFormat, externFormat, formatType;
//	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
//	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);
//
//	uint32_t levelWidth = GetWidth(level);
//	uint32_t levelHeight = GetHeight(level);
//	
//	// Compute image size
//	uint32_t imageSize = levelWidth *levelHeight * texelSize;
//	rowPitch = levelWidth * texelSize; 
//
//	int blockSize = 0;
//	if (PixelFormatUtils::IsCompressed(mFormat))
//	{
//		blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
//		imageSize = ((width+3)/4)*((height+3)/4)*blockSize; 
//	}
//
//	uint8_t* p;
//	switch(tma)
//	{
//	case RMA_Read_Only:
//	case RMA_Read_Write:
//		{
//			if (mPixelBufferID != 0) 
//			{
//				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
//				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBufferID);
//				glBufferData(GL_PIXEL_PACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
//
//				glBindTexture(mTextureTarget, mTextureOGL);
//
//				if (PixelFormatUtils::IsCompressed(mFormat))
//				{
//					glGetCompressedTexImage(mTextureTarget, level, NULL);
//					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
//				}
//				else
//				{
//					glGetTexImage(mTextureTarget, level, externFormat, formatType, NULL);
//					p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY));
//				}
//			}
//			else
//			{
//				mTextureData.resize(imageSize);
//				glBindTexture(mTextureTarget, mTextureOGL);
//				if (PixelFormatUtils::IsCompressed(mFormat))
//				{
//					glGetCompressedTexImage(mTextureTarget, level, &mTextureData[0]);
//					p = &mTextureData[0];
//				}
//				else
//				{
//					glGetTexImage(mTextureTarget, level, externFormat, formatType, &mTextureData[0]);
//					p = &mTextureData[0];
//				}
//			}
//		}
//		break;
//
//	case RMA_Write_Only:
//		{
//			if (mPixelBufferID != 0) 
//			{
//				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
//				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBufferID);
//				glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, imageSize, NULL, GL_STREAM_DRAW);
//				p = static_cast<uint8_t*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY));
//			}
//			else
//			{
//				mTextureData.resize(imageSize);
//				p = &mTextureData[0];
//			}
//		}
//		break;
//
//	default:
//		assert(false);
//	}
//
//	if (PixelFormatUtils::IsCompressed(mFormat))
//		data = p + (yOffset / 4) * rowPitch + (xOffset / 4 * blockSize);
//	else
//		data = p + (yOffset * levelWidth + xOffset) * texelSize;
//}
//
//void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
//{
//	if ( (mAccessHint & (EAH_CPU_Read | EAH_CPU_Write)) == 0 )
//		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Map only work with CPU side access!", "OpenGLTexture2D::Unmap2D");
//
//	uint32_t levelWidth = GetWidth(level);
//	uint32_t levelHeight = GetHeight(level);
//
//	switch(mTextureMapAccess)
//	{
//	case RMA_Read_Only:
//		{
//			if (mPixelBufferID != 0) 
//			{
//				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
//				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mPixelBufferID);
//				glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
//			}
//			break;
//		}
//
//	case RMA_Write_Only:
//	case RMA_Read_Write:
//		{
//			GLenum internalFormat, externFormat, formatType;
//			OpenGLMapping::Mapping(internalFormat, externFormat, formatType, mFormat);
//			uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(mFormat);
//			
//			uint32_t imageSize = 0;
//			if (PixelFormatUtils::IsCompressed(mFormat))
//			{
//				int blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
//				imageSize = ((levelWidth+3)/4)*((levelHeight+3)/4)*blockSize; 
//			}
//
//			glBindTexture(mTextureTarget, mTextureOGL);
//
//			uint8_t* p;
//			if (mPixelBufferID != 0) 
//			{
//				glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
//				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPixelBufferID);
//				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
//				p = NULL;
//			}
//			else
//				p = &mTextureData[0];
//
//			if (PixelFormatUtils::IsCompressed(mFormat))
//			{
//				if (mTextureArraySize > 1)
//					glCompressedTexSubImage3D(mTextureTarget, level, 0, 0, arrayIndex,levelWidth, levelHeight, 1, externFormat, imageSize, p);
//				else
//					glCompressedTexSubImage2D(mTextureTarget, level, 0, 0, levelWidth, levelHeight, externFormat, imageSize, p);
//			}
//			else
//			{
//				if (mTextureArraySize > 1)
//					glTexSubImage3D(mTextureTarget, level, 0, 0, arrayIndex, levelWidth, levelHeight, 1, externFormat, formatType, p);
//				else
//					glTexSubImage2D(mTextureTarget, level, 0, 0, levelWidth, levelHeight, externFormat, formatType, p);
//			}
//		}
//	}
}

void OpenGLTexture2D::Unmap2D( uint32_t arrayIndex, uint32_t level )
{

}


void OpenGLTexture2D::CopyToTexture( Texture& destTexture )
{
	/*assert(mFormat == destTexture.GetTextureFormat() && mType == destTexture.GetTextureType());
	OpenGLTexture2D& destTextureOGL = *(static_cast<OpenGLTexture2D*>(&destTexture));

	for (uint32_t arrIndex = 0; arrIndex < mTextureArraySize; ++arrIndex)
	{
		for (uint32_t level = 0; level < mMipLevels; ++level)
		{
			uint32_t levelWidth = GetWidth(level);
			uint32_t levelHeight = GetHeight(level);

			GLuint srtTex = TextureOGL;
			GLuint dstTex = destTextureOGL.TextureOGL;

			GLenum srcTarget = TextureTarget;
			GLenum dstTarget = destTextureOGL.TextureTarget;

			if (GLEW_ARB_copy_image)
			{
				glCopyImageSubData(srtTex, srcTarget, level, 0, 0, arrIndex,
							       dstTex, dstTarget, level, 0, 0, arrIndex, 
								   levelWidth, levelHeight, 1);
			}
			else
			{
				GLuint oldFBO = OpenGLFrameBuffer::GetFBO();
				{
					GLuint srcFBO, dstFBO;
					OpenGLDevice* deviceOGL = static_cast<OpenGLDevice*>(Context::GetSingleton().GetRenderDevicePtr());
					deviceOGL->GetBlitFBO(srcFBO, dstFBO);

					GLenum attachment, bufferBit;
					bool depth = PixelFormatUtils::IsDepth(GetTextureFormat());
					bool stencil = PixelFormatUtils::IsStencil(GetTextureFormat());
					if (depth || stencil)
					{
						if (depth && stencil)
						{
							attachment = GL_DEPTH_STENCIL_ATTACHMENT;
							bufferBit = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
						}
						else if (depth)
						{
							attachment = GL_DEPTH_ATTACHMENT;
							bufferBit = GL_DEPTH_BUFFER_BIT;
						}
						else if (stencil)
						{
							attachment = GL_STENCIL_ATTACHMENT;
							bufferBit = GL_STENCIL_BUFFER_BIT;
						}
					}
					else 
					{
						attachment = GL_COLOR_ATTACHMENT0;
						bufferBit = GL_COLOR_BUFFER_BIT;
					}

					glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
					if (mTextureArraySize > 1)
						glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, attachment, srtTex, level, arrIndex);
					else 
						glFramebufferTexture2D(GL_READ_FRAMEBUFFER, attachment, GL_TEXTURE_2D, srtTex, level);

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);
					if (mTextureArraySize > 1)
						glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, attachment, dstTex, level, arrIndex);
					else 
						glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, dstTex, level);

					glBlitFramebuffer(0, 0, levelWidth, levelHeight, 0, 0, levelWidth, levelHeight, bufferBit, GL_NEAREST);
				}
				OpenGLFrameBuffer::BindFBO(oldFBO);
			}
		}
	}

	OGL_ERROR_CHECK();*/
}



}


