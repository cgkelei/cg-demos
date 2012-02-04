#ifndef Renderable_h__
#define Renderable_h__

#include "Core/Prerequisites.h"
#include "Graphics/RenderOperation.h"
#include "Math/BoundingSphere.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::BoundingSpheref;

		class _ApiExport Renderable
		{
		public:
			virtual ~Renderable();
			
			virtual const String& GetName() const = 0;	
			virtual Material& GetRenderMaterial() = 0;
			virtual RenderOperation& GetRenderOperation() = 0;
			virtual const BoundingSpheref& GetBoundingSphere() const = 0;

			virtual void OnRenderBegin();
			virtual void OnRenderEnd();

		};
	}
}

#endif // Renderable_h__
