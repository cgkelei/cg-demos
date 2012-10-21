#ifndef OpenGLFrameBuffer_h__
#define OpenGLFrameBuffer_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/FrameBuffer.h>

namespace RcEngine {


class _OpenGLExport OpenGLFrameBuffer : public FrameBuffer
{
public:
	OpenGLFrameBuffer(uint32_t width, uint32_t height, bool offscreen = true);
	virtual ~OpenGLFrameBuffer();

	virtual void DoBind();
	virtual void DoUnbind();

	virtual void Resize(uint32_t width, uint32_t height);
	virtual void SwapBuffers();

	GLuint GetFrameBufferObject() const { return mFrameBufferObject; }

private:
	GLuint mFrameBufferObject;

};

}



#endif // OpenGLFrameBuffer_h__
