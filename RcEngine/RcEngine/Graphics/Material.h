#ifndef Material_h__
#define Material_h__

#include "Core/Prerequisites.h"
#include "Math/ColorRGBA.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::Color
		class _ApiExport Material 
		{
		public:
			Material(void);
			virtual ~Material(void);

		protected:			
			String mName;
			shared_ptr<Effect> mEffect;

			ColorRGBA mAmbient;
			ColorRGBA mDiffuse;
			ColorRGBA mSpecular;

			ColorRGBA mEmissive;

		};
	}
}

#endif // Material_h__

