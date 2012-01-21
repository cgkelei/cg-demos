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
			OpenGLFrameBuffer(unsigned int width, unsigned int height, bool offscreen = true);
			virtual ~OpenGLFrameBuffer();

			virtual void DoBind();
			virtual void DoUnbind();

			virtual void Clear(unsigned int flags, Math::ColorRGBA& clr, float depth, unsigned int stencil);
			virtual void SwapBuffers();

			GLuint GetFrameBufferObject() const { return mFrameBufferObject; }

		private:
			GLuint mFrameBufferObject;

		};

	}
}



#endif // OpenGLFrameBuffer_h__
