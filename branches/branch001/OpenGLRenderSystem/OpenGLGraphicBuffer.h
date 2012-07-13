#pragma once
#ifndef OpenGLGraphicBuffer_h__
#define OpenGLGraphicBuffer_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include "Graphics/GraphicBuffer.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLGraphicsBuffer : public GraphicsBuffer
		{
		public:
			OpenGLGraphicsBuffer(BufferUsage usage, uint32 accessHint, GLenum target, ElementInitData* initData);
			~OpenGLGraphicsBuffer(void); 

			GLenum GetTarget() const;
			GLuint GetBufferID() const;

			void* Map(uint32 offset, uint32 length, BufferAccess options);
			void UnMap();

		private:
			GLuint mBufferID;
			GLenum mTarget;
		};

	}
}

#endif // OpenGLGraphicBuffer_h__