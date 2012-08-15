#include <Graphics/Texture.h>

namespace RcEngine {
namespace Render {

Texture::Texture( TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint )
	: mType(type), mFormat(format), mSampleCount(sampleCount), mSampleQuality(sampleQuality), mAccessHint(accessHint)
{

}

Texture::~Texture()
{

}

} // Namespace Render
} // Namespace RcEngine