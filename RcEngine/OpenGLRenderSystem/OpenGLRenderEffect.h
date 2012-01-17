#pragma once
#ifndef OpenGLRenderEffect_h__
#define OpenGLRenderEffect_h__

#include "OpenGLPrerequisites.h"
#include "OpenGLGraphicCommon.h"
#include "Graphics/RenderEffect.h"


namespace RcEngine
{
	namespace RenderSystem
	{

		//////////////////////////////////////////////////////////////////////////

		class _OpenGLExport OpenGLRenderEffect : public RenderEffect
		{
		public:
			OpenGLRenderEffect(const std::string& effectFile, const std::string& effectName);
			~OpenGLRenderEffect(void);

			virtual bool LoadEffectFromFile(const std::string& effectFile);

			// The error handler for the Cg runtime.
			static void ErrorHandler( CGcontext context, CGerror error, void* appdata );
		protected:
			virtual void Destroy();

		private:
			CGeffect mCgEffect;
		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLRenderTechnique : public RenderTechnique
		{
		public:
			OpenGLRenderTechnique(CGtechnique technique);
			~OpenGLRenderTechnique();

		private:
			CGtechnique mTechnique;

		};

		//////////////////////////////////////////////////////////////////////////
		class _OpenGLExport OpenGLRenderPass : public RenderPass
		{
		public:
			OpenGLRenderPass(CGpass pass);
			~OpenGLRenderPass();

			bool BeginPass();
			void EndPass();

		private:
			CGpass mPass;
		};

	}
}
#endif // OpenGLRenderEffect_h__