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
	
	TargetOGL = pTextureOGL->GetTextureOGL();
	TextureTargetOGL = pTextureOGL->GetTextureTarget();
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