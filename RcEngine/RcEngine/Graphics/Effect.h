#ifndef Effect_h__
#define Effect_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectParameter.h"

namespace RcEngine
{
	namespace Render
	{
		
		/**
		 * Defines an effect which can be applied during rendering.
		 * Use renderfactory to create effect. 
		 */
		class _ApiExport Effect
		{
		public:
			Effect(const String& name);
			virtual ~Effect();

			const String& GetName() const	{ return mName; }
			bool IsValid() const			{ return mValid; }

			vector<EffectTechnique*>& GetTechniques();
			EffectTechnique* GetTechniqueByName(const String& techName);
			EffectTechnique* GetTechniqueByIndex(uint32_t index);

			EffectParameter* GetParameterByName(const String& paraName) const;
			EffectParameter* GetParameterBySemantic(const String& semanticName) const;
			EffectParameter* GetParameterByIndex(uint32_t index) const;

		protected:
			String mName;
			bool mValid;
			EffectParameterCollection mParameters;
			vector<EffectTechnique*> mTechniques;
			
		};
	}
}


#endif // Effect_h__