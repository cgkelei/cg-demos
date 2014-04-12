#include "OpenGLBuffer.h"
#include "OpenGLGraphicCommon.h"

namespace RcEngine {

OpenGLBuffer::OpenGLBuffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags, GLenum target, uint32_t structSize, ElementInitData* initData )
	: RHBuffer(bufferSize, accessHint, flags),
	  BufferOGL(0),
	  BufferTargetOGL(target)
{
	assert((GL_ARRAY_BUFFER == target)			||
		   (GL_ELEMENT_ARRAY_BUFFER == target)  ||
		   (GL_UNIFORM_BUFFER == target)		||
		   (GL_SHADER_STORAGE_BUFFER == target) ||
		   (GL_TEXTURE_BUFFER == target));

	glGenBuffers(1, &BufferOGL);
	glBindBuffer(BufferTargetOGL, BufferOGL);

	GLenum bufferUsage = OpenGLMapping::Mapping(accessHint);
	glBufferData(BufferTargetOGL, static_cast<GLsizeiptr>(bufferSize), initData ? initData->pData : nullptr, bufferUsage);

	glBindBuffer(BufferTargetOGL, 0);

	OGL_ERROR_CHECK();
}

OpenGLBuffer::~OpenGLBuffer( )
{
	if (BufferOGL > 0)
		glDeleteBuffers(1, &BufferOGL);
}

}