#include "OpenGLView.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine {

OpenGLBufferSRV::OpenGLBufferSRV( const shared_ptr<RHBuffer>& buffer )
	: mBuffer(buffer)
{

}

OpenGLTextureBufferSRV::OpenGLTextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: OpenGLBufferSRV(buffer),
	  mTextureOGL(0)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum outFormat, type;
	OpenGLMapping::Mapping(mFormatOGL, outFormat, type, format);

	glGenTextures(1, &mTextureOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mTextureOGL);
	glTexBuffer(GL_TEXTURE_BUFFER, mFormatOGL, pTBO->GetBufferOGL());
}

OpenGLTextureBufferSRV::~OpenGLTextureBufferSRV()
{
	if (mTextureOGL > 0)
		glDeleteTextures(1, &mTextureOGL);
}

void OpenGLTextureBufferSRV::BindSRV( GLuint unit )
{
	// Now bind it for read-write to one of the image units
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_BUFFER, mTextureOGL);
	
	// Not sure whether it needs to call again.
	//glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);
}

//////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferSRV::OpenGLStructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: OpenGLBufferSRV(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);
	mBufferOGL = pSSBO->GetBufferOGL();
}

void OpenGLStructuredBufferSRV::BindSRV( GLuint bindingPoint )
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, mBufferOGL);
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
		mTextureTargetOGL = pTextureOGL->GetTextureTarget();
		mTextureOGL = pTextureOGL->GetTextureOGL();
	}
	else
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mTextureOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D:
			{
				mTextureTargetOGL = GL_TEXTURE_1D;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_1D_ARRAY:
			{
				mTextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D:
			{
				mTextureTargetOGL = GL_TEXTURE_2D;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			{
				mTextureTargetOGL = GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, 0, 1);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				mTextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			{
				mTextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, 0, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_3D:
			{
				mTextureTargetOGL = GL_TEXTURE_3D;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP:
			{
				mTextureTargetOGL = GL_TEXTURE_CUBE_MAP;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			{
				mTextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
				glTextureView(mTextureOGL, mTextureTargetOGL, pTextureOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
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
		glDeleteTextures(1, &mTextureOGL);
}

void OpenGLTextureSRV::BindSRV( GLuint unit )
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(mTextureTargetOGL, mTextureOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLBufferUAV::OpenGLBufferUAV( const shared_ptr<RHBuffer>& buffer )
	: mBuffer(buffer)
{

}
//////////////////////////////////////////////////////////////////////////
OpenGLTextureBufferUAV::OpenGLTextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: OpenGLBufferUAV(buffer)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	GLenum outFormat, type;
	OpenGLMapping::Mapping(mFormatOGL, outFormat, type, format);

	glGenTextures(1, &mTextureOGL);
	glBindTexture(GL_TEXTURE_BUFFER, mTextureOGL);
	glTexBuffer(GL_TEXTURE_BUFFER, mFormatOGL, pTBO->GetBufferOGL());
}

OpenGLTextureBufferUAV::~OpenGLTextureBufferUAV()
{
	glDeleteTextures(1, &mTextureOGL);
}

void OpenGLTextureBufferUAV::BindUAV( GLuint unit )
{
	glBindImageTexture(unit, mTextureOGL, 0, GL_FALSE, 0, GL_READ_WRITE, mFormatOGL);
}

////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferUAV::OpenGLStructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: OpenGLBufferUAV(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);
	mBufferOGL = pSSBO->GetBufferOGL();
}

void OpenGLStructuredBufferUAV::BindUAV( GLuint bindingPoint )
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mBufferOGL);
}

////////////////////////////////////////////////////////////////////////
OpenGLTextureUAV::OpenGLTextureUAV( const shared_ptr<RHTexture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize )
	: mTexture(texture),
	  mLevel(mipSlice)
{
	uint32_t numLayers = texture->GetTextureArraySize();

	GLenum externFormat, formatType;
	OpenGLMapping::Mapping(mFormatOGL, externFormat, formatType, texture->GetTextureFormat());

	OpenGLTexture* pTextureOGL = static_cast_checked<OpenGLTexture*>(texture.get());
	
	if (firstArraySlice != 0 && arraySize > 1)
	{
		assert(GLEW_ARB_texture_view);
		mNeedDelete = true;

		glGenTextures(1, &mTextureOGL);
		switch (pTextureOGL->GetTextureTarget())
		{
		case GL_TEXTURE_1D_ARRAY:
			{
				glTextureView(mTextureOGL, GL_TEXTURE_1D_ARRAY, pTextureOGL->GetTextureOGL(), mFormatOGL, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		case GL_TEXTURE_2D_ARRAY:
			{
				glTextureView(mTextureOGL, GL_TEXTURE_2D_ARRAY, pTextureOGL->GetTextureOGL(), mFormatOGL, mipSlice, 1, firstArraySlice, arraySize);
			}
			break;
		default:
			assert(false);
			break;
		}
	}
	else
	{
		mFirstLayer = firstArraySlice;
		mNeedDelete = false;
	}
}

OpenGLTextureUAV::~OpenGLTextureUAV()
{
	if (mNeedDelete)
		glDeleteTextures(1, &mTextureOGL);
}

void OpenGLTextureUAV::BindUAV( GLuint unit )
{
	GLboolean layered = (mNumLayers == 1) ? GL_FALSE : GL_TRUE;
	glBindImageTexture(unit, mTextureOGL, mLevel, layered, mFirstLayer, GL_READ_WRITE, mFormatOGL);
}

}