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
};

class _OpenGLExport OpenGLTextureSRV : public RHTextureSRV
{
public:
	OpenGLTextureSRV(const shared_ptr<RHTexture>& texture);

public:
	GLenum TextureOGL;
	GLenum TextureTargetOGL;
};

class _OpenGLExport OpenGLTextureViewSRV : public OpenGLTextureSRV
{
public:
	OpenGLTextureViewSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	virtual ~OpenGLTextureViewSRV();
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLBufferUAV : public RHBufferUAV
{
public:
	OpenGLBufferUAV(const shared_ptr<RHBuffer>& buffer);
	virtual ~OpenGLBufferUAV();

public:
	GLuint BufferOGL;
	GLenum BufferTargetOGL;
};

class _OpenGLExport OpenGLTextureBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferUAV();

public:
	GLuint TextureOGL;
	GLenum FormatOGL;
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
};

class _OpenGLExport OpenGLTextureUAV : public RHTextureUAV
{
public:
	OpenGLTextureUAV(const shared_ptr<RHTexture>& texture);

public:
	GLenum TargetOGL;
	GLenum TextureTargetOGL;
};

}



#endif // OpenGLShaderResourceView_h__
