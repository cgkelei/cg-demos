#include "D3D11Texture.h"
#include "D3D11GraphicCommon.h"

namespace RcEngine {

D3D11Texture3D::D3D11Texture3D( PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t width, uint32_t height, uint32_t depth, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags, ElementInitData* initData )
	: D3D11Texture(TT_Texture3D, format, arraySize, numMipMaps, sampleCount, sampleQuality, accessHint, flags),
	  TextureD3D11(nullptr)
{

}

D3D11Texture3D::~D3D11Texture3D()
{
	SAFE_RELEASE(TextureD3D11);
}

void D3D11Texture3D::Map3D( uint32_t arrayIndex, uint32_t level, ResourceMapAccess tma, uint32_t xOffset, uint32_t yOffset, uint32_t zOffset, uint32_t width, uint32_t height, uint32_t depth, void*& data, uint32_t& rowPitch, uint32_t& slicePitch )
{

}

void D3D11Texture3D::Unmap3D( uint32_t arrayIndex, uint32_t level )
{

}





















}