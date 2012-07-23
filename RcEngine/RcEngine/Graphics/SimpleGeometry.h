#ifndef SimpleGeometry_h__
#define SimpleGeometry_h__

#include "Core/Prerequisites.h"
#include "Graphics/Renderable.h"
#include "Math/BoundingSphere.h"
#include "Math/Matrix.h"

namespace RcEngine 
{
	namespace Render
	{
		class _ApiExport RenderableHelper : public Renderable
		{
		public:
			RenderableHelper(const String& name);
			virtual ~RenderableHelper();

			const String& GetName() const								{ return mName; }
			const Math::BoundingSpheref& GetBoundingSphere() const		{ return mBoundingSphere; }

			const shared_ptr<Material>& GetMaterial() const				{ return mMaterial; }

			void GetWorldTransforms(Math::Matrix4f* xform) const		{ xform[0] = mWorldMatrix; }
			uint32_t GetWorldTransformsCount() const					{ return 1; }

			void SetWorldMatrix(const Math::Matrix4f& mat)				{ mWorldMatrix = mat; }

		protected:
			String mName;
			Math::BoundingSpheref mBoundingSphere;

			Math::Matrix4f mWorldMatrix;
			shared_ptr<Material> mMaterial;
		};

		class _ApiExport SimpleBox : public RenderableHelper
		{
		public:
			SimpleBox(const String& name);
			~SimpleBox();

			const shared_ptr<RenderOperation>& GetRenderOperation() const;
		};
		

	}
}

#endif // SimpleGeometry_h__

