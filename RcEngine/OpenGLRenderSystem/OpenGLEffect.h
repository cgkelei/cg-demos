#ifndef OpenGLEffect_h__
#define OpenGLEffect_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/Effect.h>

namespace RcEngine {

class _OpenGLExport OpenGLEffect : public Effect
{
public:
	OpenGLEffect(const String& fileName, const String& name);
	~OpenGLEffect();


public:
	static void ErrorHandler( CGcontext context, CGerror error, void* appdata );
	static shared_ptr<OpenGLEffect> LoadEffectFromFile(const String& file, const String& name);

private:
	String mEffectFile;
	CGeffect mCgEffect;
};

}


#endif // OpenGLEffect_h__