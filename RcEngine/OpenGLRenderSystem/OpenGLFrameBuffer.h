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
	virtual bool CheckFramebufferStatus();

	GLuint GetFrameBufferObject() const { return mFrameBufferObject; }

public:
	static void BindFBO(GLuint fbo);
	static GLuint GetFBO();

private:
	GLuint mFrameBufferObject;


	// Hack: keep current frame buffer binding of OpenGL system
	static GLuint msCurrFrameBufferObject;
};

}



#endif // OpenGLFrameBuffer_h__
