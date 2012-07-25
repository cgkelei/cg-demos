#ifndef OpenGLEffectTechnique_h__
#define OpenGLEffectTechnique_h__

#include "OpenGLPrerequisites.h"
#include "Graphics/EffectTechnique.h"

namespace RcEngine
{
	namespace Render
	{
		class _OpenGLExport OpenGLEffectTechnique : public EffectTechnique
		{
		public:
			OpenGLEffectTechnique(CGtechnique technique);
			~OpenGLEffectTechnique();

		private:
			CGtechnique mCgTechnique;

		};
	}
}



#endif // OpenGLEffectTechnique_h__