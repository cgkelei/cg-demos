#include "OpenGLGraphicBuffer.h"

namespace RcEngine
{
	namespace Render
	{
		OpenGLGraphicBuffer::OpenGLGraphicBuffer(BufferUsage usage, uint32 accessHint, GLenum target, ElementInitData* initData)
			: GraphicBuffer(usage, accessHint), mTarget(target)
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


		OpenGLGraphicBuffer::~OpenGLGraphicBuffer(void)
		{
			glDeleteBuffers(1, &mBufferID);
		}

		GLenum OpenGLGraphicBuffer::GetTarget() const
		{
			return mTarget;
		}

		GLuint OpenGLGraphicBuffer::GetBufferID() const
		{
			return mBufferID;
		}

		void* OpenGLGraphicBuffer::Map( uint32 offset, uint32 length, BufferAccess options )
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

		void OpenGLGraphicBuffer::UnMap()
		{
			glBindBuffer(mTarget, mBufferID);
			glUnmapBuffer(mTarget);
			glBindBuffer(mTarget, 0);
		}

	}
}

