#include <Graphics/EffectPass.h>

namespace RcEngine {



EffectPass::EffectPass()
{

}

EffectPass::~EffectPass()
{

}

EffectAnnotationList& EffectPass::GetAnnotations()
{
	return mAnnotations;
}

EffectAnnotation* EffectPass::GetAnnotationByName( const String& name )
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

EffectAnnotation* EffectPass::GetAnnotationByIndex( uint32_t index )
{
	if (index >= 0 && index < mAnnotations.size())
	{
		return mAnnotations[index];
	}
	return nullptr;
}


} // Namespace RcEngine