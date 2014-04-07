#include <Graphics/EffectTechnique.h>

namespace RcEngine {

EffectTechnique::EffectTechnique(Effect& effect)
	: mEffect(effect),
	  mValid(true)
{

}

EffectTechnique::~EffectTechnique()
{
	for (EffectPass* pass : mPasses)
		delete pass;
}

EffectPass* EffectTechnique::GetPassByName( const String& name ) const
{
	for (EffectPass* pass : mPasses)
	{
		if (pass->GetPassName() == name)
			return pass;
	}

	return nullptr;
}

EffectPass* EffectTechnique::GetPassByIndex( uint32_t index ) const
{
	if (index >= 0 && index < mPasses.size())
		return mPasses[index];

	return nullptr;
}

EffectTechnique* EffectTechnique::Clone(Effect& effect)
{
	EffectTechnique* retVal = new EffectTechnique(effect);

	retVal->mName = mName;
	retVal->mValid = mValid;

	// clone effect pass
	retVal->mPasses.resize(mPasses.size());
	for (size_t i = 0; i < mPasses.size(); ++i)
	{
		retVal->mPasses[i] = mPasses[i]->Clone(effect);
	}

	return retVal;
}

} // Namespace RcEngine