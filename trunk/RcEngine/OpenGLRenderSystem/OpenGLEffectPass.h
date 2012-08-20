#ifndef OpenGLEffectPass_h__
#define OpenGLEffectPass_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/EffectPass.h>

namespace RcEngine {

class _OpenGLExport OpenGLEffectPass : public EffectPass
{
public:
	OpenGLEffectPass(CGpass pass);
	~OpenGLEffectPass();

	bool BeginPass();
	void EndPass();

private:
	CGpass mCgPass;
};

}




#endif // OpenGLEffectPass_h__