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
	  mTextureArraySize(0)
{

}


RHShader::RHShader( ShaderType shaderType )
	: mShaderType(shaderType)
{

}


RHShaderPipeline::RHShaderPipeline()
{

}

}