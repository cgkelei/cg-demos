#ifndef D3D11Texture_h__
#define D3D11Texture_h__

#include "D3D11Prerequisites.h"
#include <Graphics/Texture.h>

namespace RcEngine {

class _D3D11Export D3D11TextureBase : public Texture
{
public:
	D3D11TextureBase(
		TextureType type,
		PixelFormat format, 
		uint32_t arraySize, 
		uint32_t numMipMaps,
		uint32_t sampleCount, 
		uint32_t sampleQuality,
		uint32_t accessHint,
		uint32_t flags);
	virtual ~D3D11TextureBase();

	virtual void BuildMipMap();

	virtual void Map1D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t width, void*& data);

	virtual void Map2D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch);

	virtual void Map3D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
		uint32_t width, uint32_t height, uint32_t depth,
		void*& data, uint32_t& rowPitch, uint32_t& slicePitch);

	virtual void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch);

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level);
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level);
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

	virtual void CopyToTexture(Texture& destTexture);

	inline uint32_t GetTextureCreateFlags() const							{ return mFlags; }
	inline ID3D11ShaderResourceView* GetD11ShaderResourceView() const		{ return mShaderResourceView; }

protected:
	/** Flags used when the texture was created */
	uint32_t mFlags;

	ID3D11ShaderResourceView* mShaderResourceView;
};

class _D3D11Export D3D11Texture1D : public D3D11TextureBase
{
public:
	D3D11Texture1D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData);
	~D3D11Texture1D();

	void Map1D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t width, void*& data);

	void Unmap1D(uint32_t arrayIndex, uint32_t level);

private:
	ID3D11Texture1D* mTexture1D;
};


class _D3D11Export D3D11Texture2D : public D3D11TextureBase
{
public:
	D3D11Texture2D(
		PixelFormat format,
		uint32_t arraySize, 
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t height,
		uint32_t sampleCount, 
		uint32_t sampleQuality, 
		uint32_t accessHint,
		uint32_t flag,
		ElementInitData* initData);

	~D3D11Texture2D();

	ID3D11Texture2D* GetTexture() const { return mTexture2D; }

	void Map2D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
			   uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
			   void*& data, uint32_t& rowPitch);

	void Unmap2D(uint32_t arrayIndex, uint32_t level);

private:
	ID3D11Texture2D* mTexture2D;

};

class _D3D11Export D3D11Texture3D : public D3D11TextureBase
{
public:
	D3D11Texture3D(
		PixelFormat format, 
		uint32_t arraySize,
		uint32_t numMipMaps,
		uint32_t width,
		uint32_t height, 
		uint32_t depth, 
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);
	~D3D11Texture3D();

	void Map3D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
		uint32_t width, uint32_t height, uint32_t depth,
		void*& data, uint32_t& rowPitch, uint32_t& slicePitch);

	void Unmap3D(uint32_t arrayIndex, uint32_t level);

private:
	ID3D11Texture3D* mTexture3D;
};

class _D3D11Export D3D11TextureCube : public D3D11TextureBase
{
public:
	D3D11TextureCube(
		PixelFormat format, 
		uint32_t arraySize, 
		uint32_t numMipMaps, 
		uint32_t width, 
		uint32_t height, 
		uint32_t sampleCount, 
		uint32_t sampleQuality,
		uint32_t accessHint, 
		uint32_t flags,
		ElementInitData* initData);

	~D3D11TextureCube();

	void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, TextureMapAccess tma,
			    uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		        void*& data, uint32_t& rowPitch);

	void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

private:
	ID3D11Texture2D* mTextureCube;
};

}


#endif // D3D11Texture_h__
