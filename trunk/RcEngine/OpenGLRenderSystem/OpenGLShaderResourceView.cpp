#include "OpenGLShaderResourceView.h"
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
	glGenTextures(1, &TextureOGL);

	GLenum outFormat, type;
	OpenGLMapping::Mapping(FormatOGL, outFormat, type, format);

	glBindTexture(GL_TEXTURE_BUFFER, TextureOGL);
	glTexBuffer(GL_TEXTURE_BUFFER, FormatOGL, buf)
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
	assert(pSSBO->BufferTargetOGL == GL_SHADER_STORAGE_BUFFER);

	BufferOGL = pSSBO->BufferOGL;
}

//////////////////////////////////////////////////////////////////////////
OpenGLTextureSRV::OpenGLTextureSRV( const shared_ptr<RHTexture>& texture )
	: RHTextureSRV(texture)
{
	OpenGLTexture* pTextureOGL = static_cast<OpenGLTexture*>(texture.get());
	
	TargetOGL = pTextureOGL->TextureOGL;
	TextureTargetOGL = pTextureOGL->TextureTarget;
}

}