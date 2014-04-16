#ifndef OpenGLShaderResourceView_h__
#define OpenGLShaderResourceView_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHView.h>

namespace RcEngine {

//enum SRVDimensionOGL
//{
//	SRV_Texture,
//	SRV_TextureBuffer,
//	SRV_ShaderStorage
//};
//
//enum UAVDimensionOGL
//{
//	UAV_Texture,
//	UAV_TextureBuffer,
//	UAV_ShaderStorage
//};

/**
 * Design solution 1: Add virtual function to OpenGLShaderResourceView, 
 *                    Let sub-class to bind self to resource unit(Texture/Image/StorageBuffer).
 *
 * Design solution 2: Add view dimension, so can cast to sub-class based on it.
 */

class _OpenGLExport OpenGLShaderResourceView : public RHShaderResourceView
{
public:
	virtual ~OpenGLShaderResourceView() {}

	// Bind resource to (Texture/StorageBuffer unit).
	virtual void BindSRV(GLuint unitOrBindingPoint) = 0;
};

class _OpenGLExport OpenGLBufferSRV : public OpenGLShaderResourceView
{
public:
	OpenGLBufferSRV(const shared_ptr<RHBuffer>& buffer);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLTextureBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferSRV();

	virtual void BindSRV(GLuint unit);

protected:
	GLuint mTextureOGL;
	GLenum mFormatOGL;
};

class _OpenGLExport OpenGLStructuredBufferSRV : public OpenGLBufferSRV
{
public:
	OpenGLStructuredBufferSRV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);

	virtual void BindSRV(GLuint bindingPoint);

protected:
	GLuint mBufferOGL;
};

class _OpenGLExport OpenGLTextureSRV : public OpenGLShaderResourceView
{
public:
	OpenGLTextureSRV(const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize);
	virtual ~OpenGLTextureSRV();
	virtual void BindSRV(GLuint unit);

protected:
	shared_ptr<RHTexture> mTexture;
	
	GLenum mTextureOGL;
	GLenum mTextureTargetOGL;
	bool mNeedDelete;
};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLUnorderedAccessView : public RHUnorderedAccessView
{
public:
	virtual ~OpenGLUnorderedAccessView() {}

	// Bind resource to (Image/StorageBuffer unit).
	virtual void BindUAV(GLuint unit) = 0;
};

class _OpenGLExport OpenGLBufferUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLBufferUAV(const shared_ptr<RHBuffer>& buffer);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

class _OpenGLExport OpenGLTextureBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLTextureBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format);
	~OpenGLTextureBufferUAV();
	
	virtual void BindUAV(GLuint unit);

protected:
	GLuint mTextureOGL;
	GLenum mFormatOGL;
};

class _OpenGLExport OpenGLStructuredBufferUAV : public OpenGLBufferUAV
{
public:
	OpenGLStructuredBufferUAV(const shared_ptr<RHBuffer>& buffer, uint32_t elementCount);

	virtual void BindUAV(GLuint bindingPoint);

protected:
	GLuint mBufferOGL;
};

class _OpenGLExport OpenGLTextureUAV : public OpenGLUnorderedAccessView
{
public:
	OpenGLTextureUAV(const shared_ptr<RHTexture>& texture);
	virtual ~OpenGLTextureUAV();

	virtual void BindUAV(GLuint unit);

protected:
	shared_ptr<RHTexture> mTexture;

	GLenum mTextureOGL;
	GLenum mTextureTargetOGL;
	bool mNeedDelete;
};

}

#endif // OpenGLShaderResourceView_h__
