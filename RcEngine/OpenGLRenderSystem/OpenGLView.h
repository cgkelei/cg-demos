#ifndef OpenGLShaderResourceView_h__
#define OpenGLShaderResourceView_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLShaderResourceView : public ShaderResourceView
{
public:
	virtual ~OpenGLShaderResourceView() {}

	// Bind resource to (Texture/StorageBuffer unit).
	void BindSRV(GLuint unitOrBindingPoint);

protected:
	GLuint mResourceOGL;
	GLenum mTargetOGL;

	// For buffer SRV, Offset and Size
	GLuint mExtraParams[2];
};

class _OpenGLExport OpenGLTextureBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferSRV();
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);
};

class _OpenGLExport OpenGLTextureSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureSRV(const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	~OpenGLTextureSRV();

	// For Internal Use
	OpenGLTextureSRV(GLuint textureOGL, GLenum targetOGL);

protected:
	bool mNeedDelete;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLUnorderedAccessView : public UnorderedAccessView
{
public:
	virtual ~OpenGLUnorderedAccessView() {}

	// Bind resource to (Image/StorageBuffer unit).
	void BindUAV(GLuint unit);

protected:
	GLuint mResourceOGL;
	GLenum mTargetOGL;
	
	GLuint mExtraParams[4];
};

class _OpenGLExport OpenGLTextureBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferUAV();
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);
};

class _OpenGLExport OpenGLTextureUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureUAV(const shared_ptr<Texture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
    ~OpenGLTextureUAV();

protected:
	GLboolean mNeedDelete;
};

}

#endif // OpenGLShaderResourceView_h__
