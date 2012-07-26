#ifndef EffectTechnique_h__
#define EffectTechnique_h__

#include "Core/Prerequisites.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectAnnotation.h"


namespace RcEngine
{
	namespace Render
	{
		class _ApiExport EffectTechnique
		{
		public:
			EffectTechnique();
			virtual ~EffectTechnique();

			const String& GetTechniqueName() const				{ return nName; }


			EffectPassList& GetPasses();
			EffectPass*	GetPassByName(const String& name);
			EffectPass* GetPassByIndex(uint32_t index);

			EffectAnnotationList& GetAnnotations();
			EffectAnnotation* GetAnnotationByName(const String& name);
			EffectAnnotation* GetAnnotationByIndex(uint32_t index);
			

		protected:
			String nName;
			bool mValid;
			EffectPassList mPasses;
			EffectAnnotationList mAnnotations;
		};
	}
}

#endif // EffectTechnique_h__