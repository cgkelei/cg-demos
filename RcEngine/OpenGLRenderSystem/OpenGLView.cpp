#include "OpenGLView.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine {

void OpenGLShaderResourceView::BindSRV( GLuint unitOrBindingPoint )
{
	if (mTargetOGL == GL_SHADER_STORAGE_BUFFER)
	{
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, unitOrBindingPoint, mResourceOGL, 
			static_cast<GLintptr>(mExtraParams[0]), static_cast<GLsizeiptr>(mExtraParams[1]));
	}
	else 
	{
		glActiveTexture(GL_TEXTURE0 + unitOrBindingPoint);
		glBindTexture(mTargetOGL, mResourceOGL);
	}
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureBufferSRV::OpenGLTextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format )
	: mBuffer(buffer)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum internalFormat, externFormat, type;
	OpenGLMapping::Mapping(internalFormat, externFormat, type, format);

	mTargetOGL = GL_TEXTURE_BUFFER;

	// offset and size
	uint32_t elementStride = PixelFormatUtils::GetNumElemBytes(format);
	mExtraParams[0] = elementStride * elementOffset;
	mExtraParams[1] = elementStride * elementWidth ;

	glGenTextures(1, &mResourceOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mResourceOGL);
	glTexBufferRange(GL_TEXTURE_BUFFER, internalFormat, pTBO->GetBufferOGL(), 
		static_cast<GLintptr>(mExtraParams[0]), static_cast<GLsizeiptr>(mExtraParams[1]));
}

OpenGLTextureBufferSRV::~OpenGLTextureBufferSRV()
{
	glDeleteTextures(1, &mResourceOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferSRV::OpenGLStructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride)
	: mBuffer(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);
	
	mTargetOGL = GL_SHADER_STORAGE_BUFFER;
	mResourceOGL = pSSBO->GetBufferOGL();
	
	// offset and size
	mExtraParams[0] = elementOffset * strutureStride;
	mExtraParams[1] = elementWidth * strutureStride;
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureSRV::OpenGLTextureSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	: mTexture(texture), 
	  mNeedDelete(false)
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t numLevels = texture->GetMipLevels();
	uint32_t numLayers = texture->GetTextureArraySize();

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, texture->GetTextureFormat());

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(texture.get());

	if (mostDetailedMip == 0 && mipLevels == numLevels && firstArraySlice == 0 && arraySize == numLayers)
	{
		mNeedDelete = false;
		mTargetOGL = pTextureOGL->GetTextureTarget();
		mResourceOGL = pTextureOGL->GetTextureOGL();
	}
	else
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mResourceOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D:
			{
				mTargetOGL = GL_TEXTURE_1D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_1D_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D:
			{
				mTargetOGL = GL_TEXTURE_2D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			{
				mTargetOGL = GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_3D:
			{
				mTargetOGL = GL_TEXTURE_3D;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP:
			{
				mTargetOGL = GL_TEXTURE_CUBE_MAP;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			{
				mTargetOGL = (arraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
				glTextureView(mResourceOGL, mTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		default:
			assert(false);
			break;
		}
	}
}

OpenGLTextureSRV::~OpenGLTextureSRV()
{
	if (mNeedDelete)
		glDeleteTextures(1, &mResourceOGL);
}

// UAV
//////////////////////////////////////////////////////////////////////////
void OpenGLUnorderedAccessView::BindUAV( GLuint unitOrBindingPoint )
{
	if (mTargetOGL == GL_TEXTURE_BUFFER)
	{
		glBindImageTexture(unitOrBindingPoint, mResourceOGL, 0, GL_FALSE, 0, GL_READ_WRITE, mExtraParams[0]);
	}	
	else if (mTargetOGL == GL_SHADER_STORAGE_BUFFER)
	{
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, unitOrBindingPoint, mResourceOGL, 
			static_cast<GLintptr>(mExtraParams[0]), static_cast<GLsizeiptr>(mExtraParams[1]));
	}
	else
	{
		GLboolean layered = (mExtraParams[2] == 1) ? GL_FALSE : GL_TRUE;
		glBindImageTexture(unitOrBindingPoint, mResourceOGL, mExtraParams[0], layered, mExtraParams[1], GL_READ_WRITE, mExtraParams[3]);
	}
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureBufferUAV::OpenGLTextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format )
	: mBuffer(buffer)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum internalFormat, externFormat, type;
	OpenGLMapping::Mapping(internalFormat, externFormat, type, format);

	mExtraParams[0] = internalFormat;

	// offset and size
	uint32_t elementStride = PixelFormatUtils::GetNumElemBytes(format);
	mExtraParams[1] = elementStride * elementOffset;
	mExtraParams[2] = elementStride * elementWidth ;

	glGenTextures(1, &mResourceOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mResourceOGL);
	glTexBufferRange(GL_TEXTURE_BUFFER, internalFormat, pTBO->GetBufferOGL(), 
		static_cast<GLintptr>(mExtraParams[1]), static_cast<GLsizeiptr>(mExtraParams[2]));
}

OpenGLTextureBufferUAV::~OpenGLTextureBufferUAV()
{
	glDeleteTextures(1, &mResourceOGL);
}

////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferUAV::OpenGLStructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride )
	: mBuffer(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);

	mTargetOGL = GL_SHADER_STORAGE_BUFFER;
	mResourceOGL = pSSBO->GetBufferOGL();

	// offset and size
	mExtraParams[0] = elementOffset * strutureStride;
	mExtraParams[1] = elementWidth * strutureStride;
}

////////////////////////////////////////////////////////////////////////
OpenGLTextureUAV::OpenGLTextureUAV( const shared_ptr<RHTexture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize )
	: mTexture(texture)
{
	mExtraParams[0] = mipSlice;
	mExtraParams[1] = firstArraySlice;
	mExtraParams[2] = arraySize;

	GLenum internalFormat, externFormat, type;
	OpenGLMapping::Mapping(internalFormat, externFormat, type, texture->GetTextureFormat());
	mExtraParams[3] = internalFormat;

	uint32_t numLayers = texture->GetTextureArraySize();
	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(texture.get());
	
	if (firstArraySlice != 0 && arraySize > 1)
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mResourceOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D_ARRAY:
			{
				glTextureView(mResourceOGL, GL_TEXTURE_1D_ARRAY, pTextureOGL->GetTextureOGL(), internalFormat, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				glTextureView(mResourceOGL, GL_TEXTURE_2D_ARRAY, pTextureOGL->GetTextureOGL(), internalFormat, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		default:
			assert(false);
			break;
		}
	}
	else
	{
		mNeedDelete = false;
	}
}

OpenGLTextureUAV::~OpenGLTextureUAV()
{
	if (mNeedDelete)
		glDeleteTextures(1, &mResourceOGL);
}

}