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

} // Namespace RcEngine