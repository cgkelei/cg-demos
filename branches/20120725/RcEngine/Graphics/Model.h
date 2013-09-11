#ifndef Model_h__
#define Model_h__

#include "Core/Prerequisites.h"
#include "Graphics/Renderable.h"
#include "Math/BoundingSphere.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::BoundingSpheref;

		class _ApiExport Model 
		{
		public:
			Model(void);
			~Model(void);
		};

	}
}

#endif // Model_h__