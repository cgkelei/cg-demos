#include "OpenGLGraphicBuffer.h"

namespace RcEngine {

OpenGLGraphicsBuffer::OpenGLGraphicsBuffer(BufferUsage usage, uint32_t accessHint, GLenum target, ElementInitData* initData)
	: GraphicsBuffer(usage, accessHint), mTarget(target)
{
	assert((GL_ARRAY_BUFFER == target) || (GL_ELEMENT_ARRAY_BUFFER == target));

	glGenBuffers(1, &mBufferID);
	if (initData != NULL)
	{
		mSizeInBytes = initData->rowPitch;
		glBindBuffer(mTarget, mBufferID);
		glBufferData(mTarget, static_cast<GLsizeiptr>(mSizeInBytes),
			initData->pData, (BU_Static == mBufferUsage) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}
	glBindBuffer(mTarget, 0);
}


OpenGLGraphicsBuffer::~OpenGLGraphicsBuffer(void)
{
	glDeleteBuffers(1, &mBufferID);
}

GLenum OpenGLGraphicsBuffer::GetTarget() const
{
	return mTarget;
}

GLuint OpenGLGraphicsBuffer::GetBufferID() const
{
	return mBufferID;
}

void* OpenGLGraphicsBuffer::Map( uint32_t offset, uint32_t length, BufferAccess options )
{
	GLenum mapFlag;
	void* pMapBuffer = NULL;

	switch(options)
	{
	case BA_Read_Only:
		mapFlag = GL_READ_ONLY;
		break;
	case BA_Write_Only:
		mapFlag = GL_WRITE_ONLY;
		break;
	case BA_Read_Write:
		mapFlag = GL_READ_WRITE;
		break;
	}

	glBindBuffer(mTarget, mBufferID);
	pMapBuffer = glMapBuffer(mTarget, mapFlag);
	glBindBuffer(mTarget, 0);
	return pMapBuffer;
}

void OpenGLGraphicsBuffer::UnMap()
{
	glBindBuffer(mTarget, mBufferID);
	glUnmapBuffer(mTarget);
	glBindBuffer(mTarget, 0);
}

void OpenGLGraphicsBuffer::ResizeBuffer( uint32_t sizeInByte )
{
	if (mSizeInBytes != sizeInByte)
	{
		glBindBuffer(mTarget, mBufferID);
		glBufferData(mTarget, static_cast<GLsizeiptr>(sizeInByte), NULL, 
			(BU_Static == mBufferUsage) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

		mSizeInBytes = sizeInByte;
	}
}

}

