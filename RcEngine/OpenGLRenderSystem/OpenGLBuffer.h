#ifndef OpenGLBuffer_h__
#define OpenGLBuffer_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLBuffer : public RHBuffer
{
public:
	OpenGLBuffer(uint32_t bufferSize, uint32_t accessHint, uint32_t flags, GLenum target, uint32_t structSize, ElementInitData* initData);
	~OpenGLBuffer(void);

public:
	GLenum BufferTargetOGL;
	GLenum BufferOGL;
};


}


#endif // OpenGLBuffer_h__
