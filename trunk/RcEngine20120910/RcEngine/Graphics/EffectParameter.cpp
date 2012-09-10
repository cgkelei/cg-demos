#include <Graphics/EffectParameter.h>

namespace RcEngine {


EffectParameter::EffectParameter()
{

}

EffectParameter::~EffectParameter()
{

}


EffectParameterCollection::EffectParameterCollection()
{

}

EffectParameterCollection::~EffectParameterCollection()
{
	for (size_t i = 0; i < mParams.size(); ++i)
	{
		Safe_Delete(mParams[i]);
	}
}

void EffectParameterCollection::Add( EffectParameter* param )
{
	mParams.push_back(param);
}

EffectParameter* EffectParameterCollection::GetParameterByIndex( uint32_t index ) const
{
	if (index >= 0 && index < mParams.size())
	{
		return mParams[index];
	}
	return nullptr;
}

EffectParameter* EffectParameterCollection::GetParameterByName( const String& name ) const
{
	if (name.empty())
	{
		return nullptr;
	}

	for (size_t i = 0; i < mParams.size(); ++i)
	{
		if (mParams[i]->GetName() == name)
		{
			return mParams[i];
		}
	}

	return nullptr;
}

EffectParameter* EffectParameterCollection::GetParameterBySemantic( const String& semantic ) const
{
	if (semantic.empty())
	{
		return nullptr;
	}

	for (size_t i = 0; i < mParams.size(); ++i)
	{
		if (mParams[i]->GetSemantic() == semantic)
		{
			return mParams[i];
		}
	}

	return nullptr;
}


} // Namespace RcEngine