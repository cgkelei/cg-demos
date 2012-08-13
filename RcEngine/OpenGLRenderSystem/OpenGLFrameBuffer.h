#ifndef OpenGLFrameBuffer_h__
#define OpenGLFrameBuffer_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/FrameBuffer.h"

namespace RcEngine
{
	namespace Render
	{

		class _OpenGLExport OpenGLFrameBuffer : public FrameBuffer
		{
		public:
			OpenGLFrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
			virtual ~OpenGLFrameBuffer();

			virtual void DoBind();
			virtual void DoUnbind();

			virtual void Resize(uint32_t width, uint32_t height);
			virtual void Clear(uint32_t flags, Math::ColorRGBA& clr, float depth, uint32_t stencil);
			virtual void SwapBuffers();

			GLuint GetFrameBufferObject() const { return mFrameBufferObject; }

		private:
			GLuint mFrameBufferObject;

		};

	}
}



#endif // OpenGLFrameBuffer_h__
