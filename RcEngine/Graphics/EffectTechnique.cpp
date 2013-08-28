#include <Graphics/EffectTechnique.h>

namespace RcEngine {

EffectTechnique::EffectTechnique()
{

}

EffectTechnique::~EffectTechnique()
{
	for (auto iter = mPasses.begin(); iter != mPasses.end(); ++iter)
	{
		delete *iter;
	}
	mPasses.clear();
}

EffectPass* EffectTechnique::GetPassByName( const String& name )
{
	auto found = std::find_if(mPasses.begin(), mPasses.end(), 
		[&name](EffectPass* pass){ return pass->GetPassName() == name;} );

	if (found != mPasses.end())
	{
		return *found;
	}

	return nullptr;
}

EffectPass* EffectTechnique::GetPassByIndex( uint32_t index )
{
	if (index >= 0 && index < mPasses.size())
	{
		return mPasses[index];
	}
	return nullptr;
}

EffectTechnique* EffectTechnique::Clone(Effect& effect)
{
	EffectTechnique* retVal = new EffectTechnique();

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