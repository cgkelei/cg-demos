#include "OpenGLSamplerState.h"

namespace RcEngine {

OpenGLSamplerState::OpenGLSamplerState( const SamplerStateDesc& desc )
	: SamplerState(desc)
{
	glGenSamplers(1, &mOGLSampler);

	glSamplerParameteri(mOGLSampler, GL_TEXTURE_WRAP_S, OpenGLMapping::Mapping(desc.AddressU));
	glSamplerParameteri(mOGLSampler, GL_TEXTURE_WRAP_T, OpenGLMapping::Mapping(desc.AddressV));
	glSamplerParameteri(mOGLSampler, GL_TEXTURE_WRAP_R, OpenGLMapping::Mapping(desc.AddressW));
	
	GLenum min, mag;
	OpenGLMapping::Mapping( min, mag, desc.Filter);
	glSamplerParameteri(mOGLSampler, GL_TEXTURE_MAG_FILTER, min);
	glSamplerParameteri(mOGLSampler, GL_TEXTURE_MIN_FILTER, mag);

	glSamplerParameterf(mOGLSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.MaxAnisotropy);

	glSamplerParameterf(mOGLSampler, GL_TEXTURE_MIN_LOD, desc.MinLOD);
	glSamplerParameterf(mOGLSampler, GL_TEXTURE_MAX_LOD, desc.MaxLOD);
	glSamplerParameterf(mOGLSampler, GL_TEXTURE_LOD_BIAS , desc.MipMapLODBias);
	
	glSamplerParameterfv(mOGLSampler, GL_TEXTURE_BORDER_COLOR, desc.BorderColor());
}

OpenGLSamplerState::~OpenGLSamplerState()
{
	glDeleteSamplers(1, &mOGLSampler);
	mOGLSampler = 0;
}

}