#ifndef OpenGLSamplerState_h__
#define OpenGLSamplerState_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include <Graphics/SamplerState.h>

namespace RcEngine {


class _OpenGLExport OpenGLSamplerState : public SamplerState
{
public:
	OpenGLSamplerState(const SamplerStateDesc& desc);
	~OpenGLSamplerState();

	GLuint GetSamplerObject() const { return mOGLSampler; }
		
private:
	GLuint mOGLSampler;
};

}

#endif // OpenGLSamplerState_h__
