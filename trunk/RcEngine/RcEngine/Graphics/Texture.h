#pragma once
#ifndef _Texture__H
#define _Texture__H

#include "Core/Prerequisites.h"
#include "GraphicsCommon.h"
#include "PixelFormat.h"

namespace RcEngine {
namespace RenderSystem {

class _ApiExport Texture
{
public:
	Texture(TextureType type, PixelFormat format, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint);
	virtual ~Texture();

	unsigned int GetSampleCount() const			{ return mSampleCount; }
	unsigned int GetSampleQuality() const		{ return mSampleQuality; }
	PixelFormat GetTextureFormat() const		{ return mFormat; }
	TextureType GetTextureType() const          { return mType; }

	virtual unsigned int GetWidth(unsigned int level) = 0;
	virtual unsigned int GetHeight(unsigned int level) = 0;
	virtual unsigned int GetDepth(unsigned int level) = 0;

	virtual void Map1D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
		unsigned int xOffset, unsigned int width, void*& data) = 0;

	virtual void Map2D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
		unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
		void*& data, unsigned int& rowPitch) = 0;

	virtual void Map3D(unsigned int arrayIndex, unsigned int level, TextureMapAccess tma,
		unsigned int xOffset, unsigned int yOffset, unsigned int zOffset,
		unsigned int width, unsigned int height, unsigned int depth,
		void*& data, unsigned int& rowPitch, unsigned int& slicePitch) = 0;

	virtual void MapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level, TextureMapAccess tma,
		unsigned int xOffset, unsigned int yOffset, unsigned int width, unsigned int height,
		void*& data, unsigned int& rowPitch) = 0;

	virtual void Unmap1D(unsigned int arrayIndex, unsigned int level) = 0;
	virtual void Unmap2D(unsigned int arrayIndex, unsigned int level) = 0;
	virtual void Unmap3D(unsigned int arrayIndex, unsigned int level) = 0;
	virtual void UnmapCube(unsigned int arrayIndex, CubeMapFace face, unsigned int level) = 0;

protected:
	
	unsigned int mSampleCount, mSampleQuality;
	unsigned int mMipMaps;
	unsigned int mTextureArraySize;
	unsigned int mAccessHint;
	PixelFormat mFormat;
	TextureType mType;
};

} // RenderSystem
} // RcEngine

#endif