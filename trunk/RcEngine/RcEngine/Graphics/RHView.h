#ifndef RHView_h__
#define RHView_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class RHBuffer;
class RHTexture;
class RHFrameBuffer;

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHShaderResourceView
{
public:
	virtual ~RHShaderResourceView();
};

class _ApiExport RHTextureSRV : public RHShaderResourceView
{
public:
	RHTextureSRV(const shared_ptr<RHTexture>& texture);
	virtual ~RHTextureSRV();

protected:
	shared_ptr<RHTexture> mTexture;
};

class _ApiExport RHBufferSRV : public RHShaderResourceView
{
public:
	RHBufferSRV(const shared_ptr<RHBuffer>& buffer);
	virtual ~RHBufferSRV();

protected:
	shared_ptr<RHBuffer> mBuffer;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHUnorderedAccessView 
{
public:
	virtual ~RHUnorderedAccessView();
};

class _ApiExport RHTextureUAV : public RHUnorderedAccessView
{
public:
	RHTextureUAV(const shared_ptr<RHTexture>& texture);
	virtual ~RHTextureUAV();

protected:
	shared_ptr<RHTexture> mTexture;
};

class _ApiExport RHBufferUAV : public RHUnorderedAccessView
{
public:
	RHBufferUAV(const shared_ptr<RHBuffer>& buffer);
	virtual ~RHBufferUAV();

protected:
	shared_ptr<RHBuffer> mBuffer;
};

}

#endif // RHView_h__
