#ifndef RHResource_h__
#define RHResource_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

struct ElementInitData
{
	const void* pData;
	uint32_t rowPitch;
	uint32_t slicePitch;
};

struct ShaderMacro
{
	String Name;
	String Definition;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHResouce
{
public:
	RHResouce();
	virtual ~RHResouce() {}

private:
	RHResouce( const RHResouce& );
	RHResouce& operator= ( const RHResouce& );
};

class _ApiExport RHShaderResourceView
{
public:
	virtual ~RHShaderResourceView() {}
};

class _ApiExport RHUnorderedAccessView 
{
public:
	virtual ~RHUnorderedAccessView() {}
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHBuffer : public RHResouce
{
public:
	RHBuffer(uint32_t bufferSize, uint32_t accessHint, uint32_t flags);
	virtual ~RHBuffer() {}

	inline uint32_t	GetAccessHint() const   { return mAccessHint; }
	inline uint32_t	GetBufferSize() const   { return mBufferSize; }
	inline uint32_t	GetCreateFlags() const	{ return mCreateFlags; }

	virtual void ResizeBuffer(uint32_t size) = 0;
	virtual void* Map(uint32_t offset, uint32_t length, ResourceMapAccess mapType) = 0;
	virtual void UnMap() = 0;

protected:
	uint32_t mBufferSize;
	uint32_t mAccessHint;
	uint32_t mCreateFlags;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHTexture : public RHResouce
{
public:
	RHTexture(
		TextureType type, 
		PixelFormat format,
		uint32_t numMipMaps,
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t flags);
	virtual ~RHTexture() {}

	inline uint32_t GetSampleCount() const				{ return mSampleCount; }
	inline uint32_t GetSampleQuality() const			{ return mSampleQuality; }
	inline PixelFormat GetTextureFormat() const			{ return mFormat; }
	inline TextureType GetTextureType() const           { return mType; }
	inline uint32_t GetTextureArraySize() const			{ return mTextureArraySize; }
	inline uint32_t GetMipLevels() const				{ return mMipLevels; }
	inline uint32_t GetCreateFlags() const				{ return mCreateFlags; }

	// return the dimension of the texture, 0 for texture with no height or depth
	inline uint32_t GetWidth(uint32_t level = 0) const		{ return mWidth >> level; }
	inline uint32_t GetHeight(uint32_t level = 0) const		{ return mHeight >> level; }
	inline uint32_t GetDepth(uint32_t level = 0) const		{ return mDepth >> level; }

	virtual void BuildMipMap() = 0;

	virtual void Map1D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType,
		uint32_t xOffset, uint32_t width, void*& data) = 0;

	virtual void Map2D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch) = 0;

	virtual void Map3D(uint32_t arrayIndex, uint32_t level, ResourceMapAccess mapType,
		uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
		uint32_t width, uint32_t height, uint32_t depth,
		void*& data, uint32_t& rowPitch, uint32_t& slicePitch) = 0;

	virtual void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, ResourceMapAccess mapType,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch) = 0;

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level) = 0;

	virtual void CopyToTexture(RHTexture& destTexture) = 0;

protected:
	// Help function used to compute mipmap levels
	static uint32_t CalculateMipmapLevels( uint32_t n );

protected:
	TextureType mType;
	PixelFormat mFormat;
	uint32_t mSampleCount, mSampleQuality;
	uint32_t mMipLevels;
	uint32_t mTextureArraySize;
	uint32_t mWidth, mHeight, mDepth;
	uint32_t mAccessHint;
	uint32_t mCreateFlags;
};

//////////////////////////////////////////////////////////////////////////
class _ApiExport RHShader : public RHResouce
{
public:
	RHShader(ShaderType shaderType);
	virtual ~RHShader() {}

	inline ShaderType GetShaderType() const	{ return mShaderType; }

	virtual bool LoadFromByteCode(const String& filename) = 0;
	virtual bool LoadFromFile(const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint = "") = 0;

protected:
	ShaderType mShaderType;
};

class _ApiExport RHShaderPipeline : public RHResouce
{
public:
	RHShaderPipeline(Effect& effect);
	virtual~ RHShaderPipeline() {}

	const shared_ptr<RHShader>& GetShader(ShaderType shaderStage) const { return mShaderStages[shaderStage]; }

	void AttachShader(const shared_ptr<RHShader>& shader);
	void DetachShader(const shared_ptr<RHShader>& shader);
	
	virtual bool LinkPipeline() = 0;
	virtual void OnBind() = 0;
	virtual void OnUnbind() = 0;

protected:
	Effect& mEffect;
	shared_ptr<RHShader> mShaderStages[ST_Count];
};

}



#endif // RHResource_h__
