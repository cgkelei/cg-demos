#ifndef RHView_h__
#define RHView_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class RHBuffer;
class RHTexture;
class RHFrameBuffer;

enum SRVResourceDimension
{
	SRV_Buffer,
	SRV_Texture,
};

enum UAVResourceDimension
{
	UAV_Buffer,
	UAV_Texture,
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHShaderResourceView
{
public:
	virtual ~RHShaderResourceView() {}

	inline SRVResourceDimension GetViewDimension() const { return mViewDimension; }

protected:
	SRVResourceDimension mViewDimension;
};

class _ApiExport RHTextureSRV : public RHShaderResourceView
{
public:
	RHTextureSRV(const shared_ptr<RHTexture>& texture);

protected:
	shared_ptr<RHTexture> mTexture;
};

class _ApiExport RHBufferSRV : public RHShaderResourceView
{
public:
	RHBufferSRV(const shared_ptr<RHBuffer>& buffer);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHUnorderedAccessView 
{
public:
	virtual ~RHUnorderedAccessView() {}

	inline UAVResourceDimension GetViewDimension() const { return mViewDimension; }

protected:
	UAVResourceDimension mViewDimension;
};

class _ApiExport RHTextureUAV : public RHUnorderedAccessView
{
public:
	RHTextureUAV(const shared_ptr<RHTexture>& texture);

protected:
	shared_ptr<RHTexture> mTexture;
};

class _ApiExport RHBufferUAV : public RHUnorderedAccessView
{
public:
	RHBufferUAV(const shared_ptr<RHBuffer>& buffer);

protected:
	shared_ptr<RHBuffer> mBuffer;
};

}

#endif // RHView_h__
