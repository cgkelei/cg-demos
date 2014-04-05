#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {

D3D11Texture2D::D3D11Texture2D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps,
							    uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality,
								uint32_t accessHint, ElementInitData* initData )
{

	// numMipMap == 0, will generate mipmap levels automatically
	mMipLevels = (numMipMaps > 0) ? numMipMaps : Texture::CalculateMipmapLevels((std::max)(width, height));
	mWidth = width;
	mHeight = height;
	
	D3D11_TEXTURE2D_DESC desc;
	D3D11Mapping::MapUsage(accessHint, desc.Usage, desc.CPUAccessFlags);
	
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.SampleDesc.Quality = sampleQuality;
	desc.SampleDesc.Count = sampleCount;
	desc.Format = 
	
}




}