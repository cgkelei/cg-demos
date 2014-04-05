#ifndef D3D11Texture_h__
#define D3D11Texture_h__

#include "D3D11Prerequisites.h"
#include <Graphics/Texture.h>

namespace RcEngine {

class _D3D11Export D3D11Texture : public Texture
{
public:
	D3D11Texture();
	virtual ~D3D11Texture();

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

protected:

};

class _D3D11Export D3D11Texture1D : public D3D11Texture
{
public:
	D3D11Texture1D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);
	~D3D11Texture1D();


private:
	ID3D11Texture1D* mTexture;
};


class _D3D11Export D3D11Texture2D : public D3D11Texture
{
public:
	D3D11Texture2D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	~D3D11Texture2D();


private:
	ID3D11Texture2D* mTexture;
};

class _D3D11Export D3D11Texture3D : public D3D11Texture
{
public:



private:
	ID3D11Texture3D* mTexture;
};

class _D3D11Export D3D11TextureCube : public D3D11Texture
{


private:
	ID3D11Texture2D* mTexture;
};

}


#endif // D3D11Texture_h__
