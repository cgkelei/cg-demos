#include "Graphics/RenderEffect.h"
#include "Graphics/RenderPatameter.h"
#include "Core/XMLDom.h"

namespace RcEngine {
namespace Render {


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
	String semantic = semanticName;
	std::transform(semantic.begin(), semantic.end(), semantic.begin(), toupper);

	ParameterList::iterator iter = mParameters.begin();
	while( iter != mParameters.end() )
	{
		if( (*iter)->HasSemantic() )
		{
			if( (*iter)->GetParameterSemantic() == semantic )
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

//void RenderEffect::Load( const String& effectFile )
//{
//	using namespace Core;
//
//	XMLDocument effectXML;
//	XMLNodePtr effectRoot = effectXML.Parse(effectFile.c_str());
//
//	// Get fx file name
//	String fxFileName = effectRoot->Attribute("name")->ValueString();
//
//	XMLAttributePtr attr = NULL;
//
//	// parse parameter
//	std::vector<XMLNodePtr> parameterNodes;
//	for (XMLNodePtr node = effectRoot->FirstNode(); node; node = node->NextSibling())
//	{
//		if ("parameter" == node->NodeName());
//		{
//			parameterNodes.push_back(node);
//		}
//		else if ( "cbuffer" == node->NodeName() )
//		{
//			for (XMLNodePtr subNode = node->FirstNode("parameter"); subNode; subNode = subNode->NextSibling("parameter"))
//			{
//				parameterNodes.push_back(subNode);
//			}
//		}
//	}
//
//	for (uint32 paramIndex = 0; paramIndex < parameterNodes.size(); ++ paramIndex)
//	{
//		XMLNodePtr const & node = parameterNodes[paramIndex];
//
//		String type = node->AttributeString("type");
//
//		if ((type != REDT_sampler)
//			&& (type != REDT_texture1D) && (type != REDT_texture2D) && (type != REDT_texture3D) && (type != REDT_textureCUBE)
//			&& (type != REDT_texture1DArray) && (type != REDT_texture2DArray) && (type != REDT_texture3DArray) && (type != REDT_textureCUBEArray))
//		{
//			XMLNodePtr parentNode = node->GetParent();
//			std::string cbuff_name = parentNode->AttributeString("name", "global_cb");
//
//			bool found = false;
//			for (size_t i = 0; i < mConstBuffers.size(); ++ i)
//			{
//				if (mConstBuffers[i].first == cbuff_name)
//				{
//					mConstBuffers[i].second.push_back(paramIndex);
//					found = true;
//					break;
//				}
//			}
//			if (!found)
//			{
//				mConstBuffers[i].push_back(std::make_pair(cbuff_name, std::vector<uint32>(1, paramIndex)));
//			}
//		}
//		
//		mParameters.push_back(new RenderParameter());
//
//		param->Load(node);
//	}
//}


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