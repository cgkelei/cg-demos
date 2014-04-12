#ifndef OpenGLShaderResourceView_h__
#define OpenGLShaderResourceView_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHView.h>

namespace RcEngine {

class RHBuffer;
class RHTexture;

class _OpenGLExport OpenGLBufferSRV : public RHBufferSRV
{
public:
	OpenGLBufferSRV(const shared_ptr<RHBuffer>& buffer);
	virtual ~OpenGLBufferSRV();

public:
	GLuint BufferOGL;
	GLenum BufferTargetOGL;
};

class _OpenGLExport OpenGLTextureBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferSRV();

public:
	GLuint TextureOGL;
	GLenum FormatOGL;
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
	~OpenGLStructuredBufferSRV();
};

class _OpenGLExport OpenGLTextureSRV : public RHTextureSRV
{
public:
	OpenGLTextureSRV(const shared_ptr<RHTexture>& texture);
	~OpenGLTextureSRV();

public:
	GLenum TargetOGL;
	GLenum TextureTargetOGL;
};

}



#endif // OpenGLShaderResourceView_h__
