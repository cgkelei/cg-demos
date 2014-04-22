#include <Graphics/RHResource.h>

namespace RcEngine {

RHResouce::RHResouce()
{

}


RHBuffer::RHBuffer( uint32_t bufferSize, uint32_t accessHint, uint32_t flags )
	: mBufferSize(bufferSize),
	  mAccessHint(accessHint),
	  mCreateFlags(flags)
{

}

RHTexture::RHTexture( TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t flags )
	: mType(type),
	  mFormat(format),
	  mSampleCount(sampleQuality),
	  mSampleQuality(sampleQuality),
	  mMipLevels(numMipMaps),
	  mAccessHint(accessHint),
	  mCreateFlags(flags),
	  mTextureArraySize(1)
{

}

uint32_t RHTexture::CalculateMipmapLevels( uint32_t n )
{
	//return int(ceil( log( float(_n) ) / log( 2.f ) )) + 1;
	//return uint32_t(1.0 + floor(log(float(n)/log(2.f))));

	uint32_t levels = 1;
	while (n >> levels) levels++;
	return levels;
}


RHShader::RHShader( ShaderType shaderType )
	: mShaderType(shaderType)
{

}

RHShaderPipeline::RHShaderPipeline( Effect& effect )
	: mEffect(effect)
{

}

}