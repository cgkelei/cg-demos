#ifndef OpenGLTexture_h__
#define OpenGLTexture_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/Texture.h>

namespace RcEngine {

class _OpenGLExport OpenGLTexture : public Texture
{
public:
	OpenGLTexture(TextureType type, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint );
	virtual ~OpenGLTexture(void);

	virtual uint32_t GetWidth(uint32_t level);
	virtual uint32_t GetHeight(uint32_t level);
	virtual uint32_t GetDepth(uint32_t level); 

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

	GLuint GetOpenGLTexture() const { return mTextureID; }
	GLenum GetOpenGLTextureTarget() const { return mTargetType; }

protected:
	GLuint mTextureID;
	GLenum mTargetType;
	std::vector<GLuint> mPixelBuffers;
	std::vector< std::vector<unsigned char> > mTextureData;
	TextureMapAccess mTextureMapAccess;
};


//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture1D : public OpenGLTexture
{
public:
	OpenGLTexture1D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	~OpenGLTexture1D();

	virtual uint32_t GetWidth(uint32_t level);

	virtual void Map1D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t width, void*& data);

	virtual void Unmap1D(uint32_t arrayIndex, uint32_t level);
private:
	std::vector<uint32_t> mWidths;

};


//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture2D : public OpenGLTexture
{
public:
	OpenGLTexture2D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	~OpenGLTexture2D();

	virtual uint32_t GetWidth(uint32_t level);
	virtual uint32_t GetHeight(uint32_t level);

	virtual void Map2D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch);

	virtual void Unmap2D(uint32_t arrayIndex, uint32_t level);

private:
	std::vector<uint32_t> mWidths;
	std::vector<uint32_t> mHeights;

};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTexture3D : public OpenGLTexture
{
public:
	OpenGLTexture3D(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t height, uint32_t depth, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);
	~OpenGLTexture3D();

	virtual uint32_t GetWidth(uint32_t level);
	virtual uint32_t GetHeight(uint32_t level);
	virtual uint32_t GetDepth(uint32_t level);

	virtual void Map3D(uint32_t arrayIndex, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t zOffset,
		uint32_t width, uint32_t height, uint32_t depth,
		void*& data, uint32_t& rowPitch, uint32_t& slicePitch);

	virtual void Unmap3D(uint32_t arrayIndex, uint32_t level);


private:
	std::vector<uint32_t> mWidths;
	std::vector<uint32_t> mHeights;
	std::vector<uint32_t> mDepths;

};

//////////////////////////////////////////////////////////////////////////
class _OpenGLExport OpenGLTextureCube : public OpenGLTexture
{
public:
	OpenGLTextureCube(PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width,
		uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, ElementInitData* initData);

	~OpenGLTextureCube();

	virtual uint32_t GetWidth(uint32_t level);
	virtual uint32_t GetHeight(uint32_t level);

	virtual void MapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level, TextureMapAccess tma,
		uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height,
		void*& data, uint32_t& rowPitch);

	virtual void UnmapCube(uint32_t arrayIndex, CubeMapFace face, uint32_t level);

private:
	std::vector<uint32_t> mSizes;

};

}

#endif // OpenGLTexture_h__
