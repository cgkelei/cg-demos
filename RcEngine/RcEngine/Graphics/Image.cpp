#include <Graphics/Image.h>
#include <Graphics/GraphicsScriptLoader.h>
#include <Core/Exception.h>

namespace RcEngine {

Image::Image()
	: mValid(false)
{

}

Image::~Image()
{
	if (mValid)
		Clear();
}


void Image::Clear()
{
	if (mValid)
	{
		uint8_t* pImageDataBase = (uint8_t*) mSurfaces.front().pData;

		delete[] pImageDataBase;
		mSurfaces.clear();
	}

	mValid = false;
}

uint32_t Image::GetRowPitch( uint32_t level )
{
	uint32_t index = (mType == TT_TextureCube) ? (level*6) : level;
	return mSurfaces[index].RowPitch;
}

uint32_t Image::GetSlicePitch( uint32_t level )
{
	assert(mType == TT_Texture3D);
	return mSurfaces[level].SlicePitch;
}

uint32_t Image::GetSurfaceSize( uint32_t level )
{
	return 0;
}

const void* Image::GetLevel( uint32_t level, uint32_t layer /*= 0*/, CubeMapFace face /*= CMF_PositiveX*/ ) const
{
	uint32_t index;

	if (mType == TT_TextureCube)
		index = layer * mLevels * CMF_Count + face * mLevels + level;
	else
		index = layer * mLevels + level;

	return mSurfaces[index].pData;
}

void* Image::GetLevel( uint32_t level, uint32_t layer /*= 0*/, CubeMapFace face /*= CMF_PositiveX*/ )
{
	uint32_t index;

	if (mType == TT_TextureCube)
		index = layer * mLevels * CMF_Count + face * mLevels + level;
	else
		index = layer * mLevels + level;

	return mSurfaces[index].pData;
}

bool Image::CopyImageFromTexture( const shared_ptr<Texture>& texture )
{
	Clear();

	mType = texture->GetTextureType();
	mFormat = texture->GetTextureFormat();
	mWidth = texture->GetWidth();
	mHeight = texture->GetHeight();
	mDepth = texture->GetDepth();
	mLevels= texture->GetMipLevels();
	mLayers = texture->GetTextureArraySize();

	uint32_t totalSize;
	uint8_t* pImageData;

	// Compute total size
	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Unimplement to save Compressed texture", "Image::CopyImageFromTexture");
	}
	else
	{
		totalSize = mWidth * mHeight * mLevels * mLayers * PixelFormatUtils::GetNumElemBytes(mFormat);

		if (mType == TT_TextureCube)
			totalSize *= 6;
		else if (mType == TT_TextureCube)
			totalSize *= mDepth;

		pImageData = new uint8_t[totalSize];
	}

	switch (mType)
	{
	case TT_Texture2D:
		{
			uint8_t* pSrcBits = pImageData;
			uint8_t* ppEndBits = pSrcBits + totalSize;

			uint32_t rowPitch;

			for (uint32_t layer = 0; layer < mLayers; ++layer)
			{
				for (uint32_t level = 0; level < mLevels; ++level)
				{
					uint32_t levelWidth = (std::max)(1U, mWidth>>level) ;
					uint32_t levelHeight = (std::max)(1U, mHeight>>level);

					void* pLevelData = pSrcBits;
					texture->Map2D(layer, level, RMA_Read_Only, 0, 0, levelWidth, levelHeight, pLevelData, rowPitch);

					// Advance 
					pSrcBits += levelHeight * rowPitch; 

					SurfaceInfo surface = { pSrcBits, rowPitch, 0 };
					mSurfaces.push_back(surface);
				}
			}

			assert(pSrcBits == ppEndBits);
		}
		break;
	case TT_Texture1D:
	case TT_Texture3D:
	case TT_TextureCube:
	default:
		break;
	}

	return true;
}


}