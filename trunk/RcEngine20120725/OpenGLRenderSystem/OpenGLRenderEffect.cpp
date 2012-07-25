#include "OpenGLRenderEffect.h"
#include "OpenGLRenderParameter.h"
#include "Core/Exception.h"

namespace RcEngine
{
	namespace Render
	{
		using Core::Exception;


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
				cgSetErrorHandler( OpenGLRenderEffect::ErrorHandler, this );
			}

		private:
			CGcontext context_;
		};



		//////////////////////////////////////////////////////////////////////////
		bool OpenGLRenderEffect::LoadEffectFromFile( const std::string& effectFile )
		{
			bool bSuccess = false;

			CGeffect effect = cgCreateEffectFromFile( CGContextIniter::Instance().Context(), effectFile.c_str(), NULL );

			if ( effect != NULL )
			{
				//Destroy();  // Destroy the current effect as we will be loading a new one.

				// Replace the existing effect.
				mCgEffect = effect;

				// Get the parameters
				CGparameter parameter = cgGetFirstEffectParameter( mCgEffect );
				while ( parameter != NULL )
				{
					OpenGLRenderParameter* pParameter = new OpenGLRenderParameter( parameter );
					mParameters.push_back(pParameter);
					parameter = cgGetNextParameter( parameter );
				}

				// Get the techniques
				CGtechnique technique = cgGetFirstTechnique( mCgEffect );
				while ( technique != NULL )
				{
					OpenGLRenderTechnique* pTechnique = new OpenGLRenderTechnique(technique);
					mTechniques.push_back(pTechnique);
					technique = cgGetNextTechnique(technique);
				}

				mIsValid = true;
				bSuccess = true;
			}

			// Store the filename so we can reload it later if necessary
			m_EffectFile = effectFile;

			return bSuccess;
		}

		OpenGLRenderEffect::~OpenGLRenderEffect( void )
		{

		}

		OpenGLRenderEffect::OpenGLRenderEffect(const std::string& effectFile, const std::string& effectName)
			: RenderEffect(effectFile, effectName)
		{
			if(!effectName.empty())
			{
				LoadEffectFromFile(effectFile);
			}
		}

		void OpenGLRenderEffect::Destroy()
		{
			mTechniques.clear();
			mParameters.clear();

			if ( mCgEffect != NULL )
			{
				cgDestroyEffect(mCgEffect);
				mCgEffect = NULL;
			}
		}

		void OpenGLRenderEffect::ErrorHandler( CGcontext context, CGerror error, void* appdata )
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

				ENGINE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, strError, "OpenGLRenderEffect::ErrorHandler");
			}
		}

		//////////////////////////////////////////////////////////////////////////
		OpenGLRenderTechnique::OpenGLRenderTechnique( CGtechnique technique )
			: mTechnique(technique)
		{
			// Validate the technique
			mIsValid = ( mTechnique != NULL ) && ( cgValidateTechnique(mTechnique) == CG_TRUE );
			if ( mTechnique != NULL )
			{
				const char* techniqueName = cgGetTechniqueName(mTechnique);
				if ( techniqueName != NULL )
				{
					mTechniqueName = techniqueName;
				}

				// Get all the passes for this technique
				CGpass pass = cgGetFirstPass( mTechnique );
				while ( pass != NULL )
				{
					OpenGLRenderPass* pPass = new OpenGLRenderPass( pass );
					mPasses.push_back(pPass);
					pass = cgGetNextPass( pass );
				}
			}
		}

		OpenGLRenderTechnique::~OpenGLRenderTechnique()
		{

		}

		//////////////////////////////////////////////////////////////////////////
		OpenGLRenderPass::OpenGLRenderPass( CGpass pass )
			:mPass(pass)
		{
			if ( mPass != NULL )
			{
				mIsValid = true;
				const char* passName = cgGetPassName(mPass);
				if ( passName != NULL )
				{
					mPassName = passName;
				}
			}
		}


		OpenGLRenderPass::~OpenGLRenderPass()
		{

		}


		bool OpenGLRenderPass::BeginPass()
		{
			if ( mIsValid )
			{
				cgSetPassState( mPass );
			}
			return mIsValid;
		}

		void OpenGLRenderPass::EndPass()
		{
			if ( mIsValid )
			{
				cgResetPassState( mPass );
			}
		}

	}
}