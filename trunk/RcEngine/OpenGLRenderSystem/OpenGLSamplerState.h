#ifndef OpenGLSamplerState_h__
#define OpenGLSamplerState_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/RHState.h>

namespace RcEngine {

class _OpenGLExport OpenGLSamplerState : public RHSamplerState
{
public:
	OpenGLSamplerState(const RHSamplerStateDesc& desc);
	~OpenGLSamplerState();

	inline GLuint GetSamplerOGL() const { return mSamplerOGL; }

private:
	GLuint mSamplerOGL;
};

}

#endif // OpenGLSamplerState_h__
