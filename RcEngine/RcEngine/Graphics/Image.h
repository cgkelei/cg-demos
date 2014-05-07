#ifndef Image_h__
#define Image_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/PixelFormat.h>

namespace RcEngine {

class _ApiExport Image 
{
public:
	Image();
	~Image();

	bool LoadFromDDS(const String& filename);

	inline uint32_t GetWidth() const		{ assert(mValid); return mWidth; }
	inline uint32_t GetHeight() const		{ assert(mValid); return mHeight; }
	inline uint32_t GetDepth() const		{ assert(mValid); return mDepth; }
	inline uint32_t GetLevels() const		{ assert(mValid); return mLevels; }
	inline uint32_t GetLayers() const		{ assert(mValid); return mLayers; }

	inline PixelFormat GetFormat() const	{ assert(mValid); return mFormat; }
	inline TextureType GetType() const		{ assert(mValid); return mType; }

	uint32_t GetRowPitch(uint32_t level);
	uint32_t GetSlicePitch(uint32_t level);
	uint32_t GetSurfaceSize(uint32_t level);
	
	const void* GetLevel(uint32_t level, uint32_t layer = 0, CubeMapFace face = CMF_PositiveX) const;
	void* GetLevel(uint32_t level, uint32_t layer = 0, CubeMapFace face = CMF_PositiveX);

private:
	void Clear();

private:

	TextureType mType;
	PixelFormat mFormat;

	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mDepth;

	uint32_t mLevels;
	uint32_t mLayers;

	struct SurfaceInfo
	{
		uint8_t* pData;
		uint32_t RowPitch;
		uint32_t SlicePitch;
	};
	std::vector<SurfaceInfo> mSurfaces;
	
	bool mValid;
};

}


#endif // Image_h__
