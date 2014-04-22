#ifndef OpenGLShaderResourceView_h__
#define OpenGLShaderResourceView_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLShaderResourceView : public RHShaderResourceView
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
	OpenGLTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferSRV();

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	~OpenGLTextureSRV();

protected:
	shared_ptr<RHTexture> mTexture;
	bool mNeedDelete;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLUnorderedAccessView : public RHUnorderedAccessView
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
	OpenGLTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, PixelFormat format);
	~OpenGLTextureBufferUAV();

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementOffset, uint32_t elementWidth, uint32_t strutureStride);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureUAV(const shared_ptr<RHTexture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
    ~OpenGLTextureUAV();

protected:
	shared_ptr<RHTexture> mTexture;
	GLboolean mNeedDelete;
};

}

#endif // OpenGLShaderResourceView_h__
