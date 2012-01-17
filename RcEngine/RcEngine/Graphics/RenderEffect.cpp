#include "RenderEffect.h"
#include "RenderPatameter.h"

namespace RcEngine {
namespace RenderSystem {


//////////////////////////////////////////////////////////////////////////
RenderEffect::RenderEffect( const std::string& effectFile, const std::string& effectName )
	: mEffectName(effectName)
{
}

RenderEffect::~RenderEffect( void )
{

}


RenderEffect::TechniqueList& RenderEffect::GetTechniques()
{
	return mTechniques;
}

RenderEffect::ParameterList& RenderEffect::GetParameters()
{
	return mParameters;
}

RenderTechnique* RenderEffect::GetTechniqueByName( const std::string& techName )
{
	TechniqueList::iterator iter = mTechniques.begin();
	while( iter != mTechniques.end() )
	{
		if( (*iter)->GetTechniqueName() == techName )
		{
			return (*iter);
		}
		++iter;
	}
	return NULL;
}

RenderParameter* RenderEffect::GetParameterByName( const std::string& paraName )
{
	ParameterList::iterator iter = mParameters.begin();
	while( iter != mParameters.end() )
	{
		if( (*iter)->GetParameterName() == paraName )
		{
			return (*iter);
		}
		++iter;
	}
	return NULL;
}

RenderParameter* RenderEffect::GetParameterBySemantic( const std::string& semanticName )
{
	ParameterList::iterator iter = mParameters.begin();
	while( iter != mParameters.end() )
	{
		if( (*iter)->HasSemantic() )
		{
			if( (*iter)->GetParameterSemantic() == semanticName )
				return (*iter);
		}
		++iter;
	}
	return NULL;
}

bool RenderEffect::IsValid() const
{
	return mIsValid;
}


//////////////////////////////////////////////////////////////////////////
RenderTechnique::RenderTechnique()
{

}

const std::string& RenderTechnique::GetTechniqueName() const
{
	return mTechniqueName;
}

bool RenderTechnique::IsValid() const
{
	return mIsValid;
}

RenderTechnique::PassList RenderTechnique::GetPasses()
{
	return mPasses;
}

RenderPass* RenderTechnique::GetPassByName( const std::string& passName )
{
	PassList::iterator iter = mPasses.begin();
	while(iter != mPasses.end())
	{
		if( (*iter)->GetPassName() == passName )
		{
			return (*iter);
		}
		++iter;
	}
	return NULL;
}

RenderTechnique::~RenderTechnique()
{

}


//////////////////////////////////////////////////////////////////////////
RenderPass::RenderPass()
	:mIsValid(false)
{

}


RenderPass::~RenderPass()
{

}

const std::string& RenderPass::GetPassName() const
{
	return mPassName;
}

bool RenderPass::IsValid() const
{
	return mIsValid;
}

} // RenderSystem
} // RcEngine