#include "OpenGLEffectPass.h"
#include "OpenGLEffectParameter.h"

namespace RcEngine {

OpenGLEffectPass::OpenGLEffectPass( CGpass pass )
	: mCgPass(pass)
{
	if ( mCgPass != NULL )
	{
		mValid = true;
		const char* passName = cgGetPassName(mCgPass);
		if ( passName != NULL )
		{
			mName = passName;
		}
	}
}

OpenGLEffectPass::~OpenGLEffectPass()
{

}

bool OpenGLEffectPass::BeginPass()
{
	if ( mValid )
	{
		cgSetPassState( mCgPass );
	}
	return mValid;
}

void OpenGLEffectPass::EndPass()
{
	if ( mValid )
	{
		cgResetPassState( mCgPass );
	}
}

}