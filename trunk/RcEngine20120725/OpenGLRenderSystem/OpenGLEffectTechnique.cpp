#include "OpenGLEffectTechnique.h"
#include "OpenGLEffectPass.h"

namespace RcEngine
{
	namespace Render
	{

		OpenGLEffectTechnique::OpenGLEffectTechnique( CGtechnique technique )
			:mCgTechnique(technique)
		{
			// Validate the technique
			mValid = ( mCgTechnique != NULL ) && ( cgValidateTechnique(mCgTechnique) == CG_TRUE );
			if ( mCgTechnique != NULL )
			{
				const char* techniqueName = cgGetTechniqueName(mCgTechnique);
				if ( techniqueName  )
				{
					nName = techniqueName;
				}

				// Get all the passes for this technique
				CGpass pass = cgGetFirstPass( mCgTechnique );
				while ( pass != NULL )
				{
					OpenGLEffectPass* pPass = new OpenGLEffectPass( pass );
					mPasses.push_back(pPass);
					pass = cgGetNextPass( pass );
				}
			}
		}

		OpenGLEffectTechnique::~OpenGLEffectTechnique()
		{

		}

	}
}