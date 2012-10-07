#pragma once
#ifndef _Texture__H
#define _Texture__H

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>
#include <Resource/Resource.h>

namespace RcEngine {

class _ApiExport Texture
{
public:
	Texture(TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint);
	virtual ~Texture();

	uint32_t GetSampleCount() const			{ return mSampleCount; }
	uint32_t GetSampleQuality() const		{ return mSampleQuality; }
	PixelFormat GetTextureFormat() const		{ return mFormat; }
	TextureType GetTextureType() const          { return mType; }

	virtual uint32_t GetWidth(uint32_t level) = 0;
	virtual uint32_t GetHeight(uint32_t level) = 0;
	virtual uint32_t GetDepth(uint32_t level) = 0;

	virtual void Map1D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t width, void*& data) = 0;

	virtual void Map2D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch) = 0;

	virtual void Map3D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
		uint32_t width, uint32_t height, uint32_t depth,
		void*& data, uint32_t& rowPitch, uint32_t& slicePitch) = 0;

	virtual void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch) = 0;

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level) = 0;
	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level) = 0;

protected:

	uint32_t mSampleCount, mSampleQuality;
	uint32_t mMipMaps;
	uint32_t mTextureArraySize;
	uint32_t mAccessHint;
	PixelFormat mFormat;
	TextureType mType;
};


class TextureResource : public Resource
{
public:
	TextureResource(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~TextureResource();


	const shared_ptr<Texture> GetTexture() const { return mTexture; }

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	void LoadImpl();
	void UnloadImpl();
	
private:
	shared_ptr<Texture> mTexture;

};

} // Namespace RcEngine

#endif