#include "OpenGLEffect.h"
#include "OpenGLEffectParameter.h"
#include "OpenGLEffectTechnique.h"
#include <Core/Exception.h>


namespace RcEngine {

class CGContextIniter
{
public:
	static CGContextIniter& Instance()
	{
		static CGContextIniter initer;
		return initer;
	}

	CGcontext Context()
	{
		return context_;
	}

private:
	CGContextIniter()
	{
		context_ = cgCreateContext();
		cgGLRegisterStates(context_);
		cgGLSetDebugMode(CG_FALSE);
		cgSetParameterSettingMode(context_, CG_DEFERRED_PARAMETER_SETTING);
		cgSetErrorHandler( OpenGLEffect::ErrorHandler, this );
	}

private:
	CGcontext context_;
};


OpenGLEffect::OpenGLEffect( const String& fileName, const String& name )
	: Effect(name), mEffectFile(fileName)
{

}

OpenGLEffect::~OpenGLEffect()
{
			
}

void OpenGLEffect::ErrorHandler( CGcontext context, CGerror error, void* appdata )
{
	if ( error != CG_NO_ERROR )
	{
		const char* pStr = cgGetErrorString(error);
		std::string strError = ( pStr == NULL ) ? "" : pStr;

		std::string strListing;
		if ( error == CG_COMPILER_ERROR )
		{
			pStr = cgGetLastListing( context );
			strListing = ( pStr == NULL ) ? "" : pStr;

			std::cerr << strListing << std::endl;
		}

		ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, strError, "OpenGLEffect::ErrorHandler");
	}
}

shared_ptr<OpenGLEffect> OpenGLEffect::LoadEffectFromFile( const String& file, const String& name )
{
	CGeffect cgHandle = cgCreateEffectFromFile( CGContextIniter::Instance().Context(), file.c_str(), NULL );
	if ( cgHandle != NULL )
	{
		shared_ptr<OpenGLEffect> effect = std::make_shared<OpenGLEffect>(file, name);
		effect->mCgEffect = cgHandle;

		// Get the parameters
		CGparameter parameter = cgGetFirstEffectParameter( cgHandle );
		while ( parameter != NULL )
		{
			OpenGLEffectParameter* pParameter = new OpenGLEffectParameter( parameter );
			effect->mParameters.Add(pParameter);
			parameter = cgGetNextParameter( parameter );
		}

		// Get the techniques
		CGtechnique technique = cgGetFirstTechnique( cgHandle );
		while ( technique != NULL )
		{
			OpenGLEffectTechnique* pTechnique =  new OpenGLEffectTechnique(technique);
			effect->mTechniques.push_back(pTechnique);
			technique = cgGetNextTechnique(technique);
		}

		effect->mValid = true;

		return effect;
	}
	return nullptr;
}

}