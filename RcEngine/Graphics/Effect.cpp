#include "Graphics/Effect.h"

namespace RcEngine
{
	namespace Render
	{
		Effect::Effect(const String& name)
			:  mName(name)
		{

		}

		Effect::~Effect()
		{
			for ( EffectTechniqueList::iterator iter = mTechniques.begin(); iter != mTechniques.end(); ++iter )
			{
				Safe_Delete(*iter);
			}
		}

		EffectTechniqueList& Effect::GetTechniques()
		{
			return mTechniques;
		}

		EffectTechnique* Effect::GetTechniqueByName( const String& techName )
		{
			EffectTechniqueList::iterator iter = mTechniques.begin();
			while( iter != mTechniques.end() )
			{
				if( (*iter)->GetTechniqueName() == techName )
				{
					return (*iter);
				}
				++iter;
			}
			return nullptr;
		}

		EffectTechnique* Effect::GetTechniqueByIndex( uint32_t index )
		{
			if (index >= 0 && index < mTechniques.size())
			{
				return mTechniques[index];
			}
			return nullptr;
		}


		EffectParameter* Effect::GetParameterByName( const String& paraName ) const
		{
			return mParameters.GetParameterByName(paraName);
		}

		EffectParameter* Effect::GetParameterBySemantic( const String& semanticName ) const
		{
			return mParameters.GetParameterBySemantic(semanticName);
		}

		EffectParameter* Effect::GetParameterByIndex(uint32_t index) const
		{
			return mParameters.GetParameterByIndex(index);
		}

	}
}