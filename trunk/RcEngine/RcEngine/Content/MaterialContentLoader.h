#ifndef MaterialContentLoader_h__
#define MaterialContentLoader_h__

#include "Core/Prerequisites.h"

namespace RcEngine
{
	namespace Content
	{
		class _ApiExport MaterialContentLoader
		{
		public:
			MaterialContentLoader(const String& path);
			~MaterialContentLoader();

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

		};
	}
}


#endif // MaterialContentLoader_h__
