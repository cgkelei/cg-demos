#ifndef MaterialContentLoader_h__
#define MaterialContentLoader_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"

namespace RcEngine
{
	namespace Content
	{
		class _ApiExport MaterialContent
		{
		public:
			MaterialContent();
			MaterialContent(const String& path);
			~MaterialContent();

		struct ParamDesc
		{
			String Type;
			String Name;
			String Semantic;
			String Annotation;
		};

		public:
			String MaterialName;
			String EffectName;
			String EffectPath;
			vector<ParamDesc> Params;


			Math::ColorRGBA Ambient;
			Math::ColorRGBA Diffuse;
			Math::ColorRGBA Specular;
			Math::ColorRGBA Emissive;
			float Power;

			unordered_map<String, String> mTextures;
		};
	}
}


#endif // MaterialContentLoader_h__
