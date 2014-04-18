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
};

class _OpenGLExport OpenGLBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLBufferSRV(const shared_ptr<RHBuffer>& buffer) : mBuffer(buffer) {}

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferSRV();
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
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

	// For Texture and TBuffer UAV
	GLenum mFormatOGL;
	GLuint mLevel; 
	GLuint mFirstLayer;
	GLuint mNumLayers;
};

class _OpenGLExport OpenGLBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLBufferUAV(const shared_ptr<RHBuffer>& buffer) : mBuffer(buffer) {}

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferUAV();
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);
};

class _OpenGLExport OpenGLTextureUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureUAV(const shared_ptr<RHTexture>& texture, uint32_t mipSlice, uint32_t firstArraySlice, uint32_t arraySize);
	virtual ~OpenGLTextureUAV();

protected:
	shared_ptr<RHTexture> mTexture;
	GLboolean mNeedDelete;
};

}

#endif // OpenGLShaderResourceView_h__
