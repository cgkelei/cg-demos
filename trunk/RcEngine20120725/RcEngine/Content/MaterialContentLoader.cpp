#include "Content/MaterialContentLoader.h"
#include "Core/XMLDom.h"
#include "Graphics/GraphicsCommon.h"

namespace RcEngine
{
	namespace Content
	{
		using namespace Core;

		MaterialContent::MaterialContent(const String& path)
		{
			XMLDocument material;
			XMLNodePtr root = material.Parse(path);
			XMLNodePtr node;

			for (node = root->FirstNode(); node; node = node->NextSibling())
			{
				if (node->NodeName() == "Parameter")
				{
					ParamDesc param;
					param.Type = node->AttributeString("type", "");
					param.Name = node->AttributeString("name", "");
					param.Semantic = node->AttributeString("semantic", "");
					param.Annotation = node->AttributeString("annotation", "");
					Params.push_back(param);
				}
			}

			node = root->FirstNode("Effect");
			EffectName = node->AttributeString("name", "");
			EffectPath = node->AttributeString("file", "");		
		}

		MaterialContent::MaterialContent()
		{
			mTextures.rehash(4);
		}

		MaterialContent::~MaterialContent()
		{

		}
	}
}