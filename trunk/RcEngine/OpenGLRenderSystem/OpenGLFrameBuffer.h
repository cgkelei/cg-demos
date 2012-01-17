#ifndef OpenGLFrameBuffer_h__
#define OpenGLFrameBuffer_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/FrameBuffer.h"

namespace RcEngine
{
	namespace RenderSystem
	{

		class _OpenGLExport OpenGLFrameBuffer : public FrameBuffer
		{
		public:
			OpenGLFrameBuffer(bool offscreen);
			virtual ~OpenGLFrameBuffer();

			GLuint GetFrameBufferObject() const { return mFrameBufferObject; }

		private:
			GLuint mFrameBufferObject;

		};

	}
}



#endif // OpenGLFrameBuffer_h__
