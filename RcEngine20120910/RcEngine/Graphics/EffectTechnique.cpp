#include <Graphics/EffectTechnique.h>

namespace RcEngine {


EffectTechnique::EffectTechnique()
{

}

EffectTechnique::~EffectTechnique()
{

}

EffectPassList& EffectTechnique::GetPasses()
{
	return mPasses;
}

EffectPass*	EffectTechnique::GetPassByName(const String& name)
{
	EffectPassList::iterator iter = mPasses.begin();
	while(iter != mPasses.end())
	{
		if ((*iter)->GetPassName() == name)
		{
			return *iter;
		}
	}
	return nullptr;
}

EffectPass* EffectTechnique::GetPassByIndex(uint32_t index)
{
	if (index >= 0 && index < mPasses.size())
	{
		return mPasses[index];
	}
	return nullptr;
}

EffectAnnotationList& EffectTechnique::GetAnnotations()
{
	return mAnnotations;
}

EffectAnnotation* EffectTechnique::GetAnnotationByName( const String& name )
{
	EffectAnnotationList::iterator iter = mAnnotations.begin();
	while(iter != mAnnotations.end())
	{
		if ((*iter)->GetAnnotationName() == name)
		{
			return *iter;
		}
	}
	return nullptr;
}

EffectAnnotation* EffectTechnique::GetAnnotationByIndex( uint32_t index )
{
	if (index >= 0 && index < mPasses.size())
	{
		return mAnnotations[index];
	}
	return nullptr;
}


} // Namespace RcEngine