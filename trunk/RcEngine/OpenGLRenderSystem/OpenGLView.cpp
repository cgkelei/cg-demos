#include "OpenGLView.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine {

OpenGLBufferSRV::OpenGLBufferSRV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferSRV(buffer),
	  BufferOGL(0)
{

}

OpenGLBufferSRV::~OpenGLBufferSRV()
{

}

OpenGLTextureBufferSRV::OpenGLTextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: OpenGLBufferSRV(buffer),
	  TextureOGL(0)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	glGenTextures(1, &TextureOGL);

	GLenum outFormat, type;
	OpenGLMapping::Mapping(FormatOGL, outFormat, type, format);
	
	BufferTargetOGL = GL_TEXTURE_BUFFER;
	BufferOGL = pTBO->GetBufferOGL();

	glBindTexture(GL_TEXTURE_BUFFER, TextureOGL);
	glTexBuffer(GL_TEXTURE_BUFFER, FormatOGL, BufferOGL);
}

OpenGLTextureBufferSRV::~OpenGLTextureBufferSRV()
{
	if (TextureOGL > 0)
		glDeleteTextures(1, &TextureOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferSRV::OpenGLStructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: OpenGLBufferSRV(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);

	BufferTargetOGL = GL_SHADER_STORAGE_BUFFER;
	BufferOGL = pSSBO->GetBufferOGL();
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureSRV::OpenGLTextureSRV( const shared_ptr<RHTexture>& texture )
	: RHTextureSRV(texture)
{
	OpenGLTexture* pTextureOGL = static_cast<OpenGLTexture*>(texture.get());
	
	TextureOGL = pTextureOGL->GetTextureOGL();
	TextureTargetOGL = pTextureOGL->GetTextureTarget();
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureViewSRV::OpenGLTextureViewSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	: OpenGLTextureSRV(texture)
{
	assert(mipLevels > 0 && arraySize > 0);

	OpenGLTexture* pTextureInternalOGL = static_cast<OpenGLTexture*>(texture.get());

	GLenum internalFormat, externFormat, formatType;
	OpenGLMapping::Mapping(internalFormat, externFormat, formatType, texture->GetTextureFormat());

	// check mipmap levels
	if (mipLevels == -1) // -1 for all remain levels
	{
		assert(mostDetailedMip < mipLevels);
		mipLevels = pTextureInternalOGL->GetMipLevels() - mostDetailedMip;
	}
	else
	{
		assert(mostDetailedMip + mipLevels <= pTextureInternalOGL->GetMipLevels());
	}

	// Check array size
	assert(firstArraySlice + arraySize <=  pTextureInternalOGL->GetTextureArraySize());
	
	glGenTextures(1, &TextureOGL);
	switch (pTextureInternalOGL->GetTextureTarget())
	{
	case GL_TEXTURE_1D:
		{
			TextureTargetOGL = GL_TEXTURE_1D;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
		}
		break;
	case GL_TEXTURE_1D_ARRAY:
		{
			TextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
		}
		break;
	case GL_TEXTURE_2D:
		{
			TextureTargetOGL = GL_TEXTURE_2D;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
		}
		break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		{
			TextureTargetOGL = GL_TEXTURE_2D_MULTISAMPLE;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, 0, 1, 0, 1);
		}
		break;
	case GL_TEXTURE_2D_ARRAY:
		{
			TextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
		}
		break;
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
		{
			TextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, 0, 1, firstArraySlice, arraySize);
		}
		break;
	case GL_TEXTURE_3D:
		{
			TextureTargetOGL = GL_TEXTURE_3D;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
		}
		break;
	case GL_TEXTURE_CUBE_MAP:
		{
			TextureTargetOGL = GL_TEXTURE_CUBE_MAP;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, 0, 1);
		}
		break;
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		{
			TextureTargetOGL = (arraySize > 1) ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
			glTextureView(TextureOGL, TextureTargetOGL, pTextureInternalOGL->GetTextureOGL(), internalFormat, mostDetailedMip, mipLevels, firstArraySlice, arraySize);
		}
		break;
	default:
		assert(false);
		break;
	}
}

OpenGLTextureViewSRV::~OpenGLTextureViewSRV()
{
	glDeleteTextures(1, &TextureOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLBufferUAV::OpenGLBufferUAV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferUAV(buffer),
	  BufferOGL(0)  
{

}

OpenGLBufferUAV::~OpenGLBufferUAV()
{

}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureBufferUAV::OpenGLTextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: OpenGLBufferUAV(buffer)
{
	OpenGLBuffer* pTBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pTBO->GetBufferTarget() == GL_TEXTURE_BUFFER);

	glGenTextures(1, &TextureOGL);

	GLenum outFormat, type;
	OpenGLMapping::Mapping(FormatOGL, outFormat, type, format);
	
	BufferTargetOGL = GL_TEXTURE_BUFFER;
	BufferOGL = pTBO->GetBufferOGL();

	glBindTexture(GL_TEXTURE_BUFFER, TextureOGL);
	glTexBuffer(GL_TEXTURE_BUFFER, FormatOGL, BufferOGL);
}

OpenGLTextureBufferUAV::~OpenGLTextureBufferUAV()
{
	if (TextureOGL > 0)
		glDeleteTextures(1, &TextureOGL);
}

//////////////////////////////////////////////////////////////////////////
OpenGLStructuredBufferUAV::OpenGLStructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: OpenGLBufferUAV(buffer)
{
	OpenGLBuffer* pSSBO = static_cast<OpenGLBuffer*>(buffer.get());
	assert(pSSBO->GetBufferTarget() == GL_SHADER_STORAGE_BUFFER);

	BufferTargetOGL = GL_SHADER_STORAGE_BUFFER;
	BufferOGL = pSSBO->GetBufferOGL();
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureUAV::OpenGLTextureUAV( const shared_ptr<RHTexture>& texture )
	: RHTextureUAV(texture)
{
	OpenGLTexture* pTextureOGL = static_cast<OpenGLTexture*>(texture.get());

	TargetOGL = pTextureOGL->GetTextureOGL();
	TextureTargetOGL = pTextureOGL->GetTextureTarget();
}

}