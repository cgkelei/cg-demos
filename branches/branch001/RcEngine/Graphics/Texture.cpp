#include "Texture.h"

namespace RcEngine {
namespace Render {

Texture::Texture( TextureType type, PixelFormat format, unsigned int numMipMaps, unsigned int sampleCount, unsigned int sampleQuality, unsigned int accessHint )
	: mType(type), mFormat(format), mSampleCount(sampleCount), mSampleQuality(sampleQuality), mAccessHint(accessHint)
{

}

Texture::~Texture()
{

}

} // RenderSystem
} // RcEngine