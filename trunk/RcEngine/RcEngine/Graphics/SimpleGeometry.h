#ifndef SimpleGeometry_h__
#define SimpleGeometry_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/BoundingSphere.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport RenderableHelper : public Renderable
{
public:
	RenderableHelper(const String& name);
	virtual ~RenderableHelper();

	const String& GetName() const								{ return mName; }
	const BoundingSpheref& GetBoundingSphere() const		{ return mBoundingSphere; }

	const shared_ptr<Material>& GetMaterial() const				{ return mMaterial; }

	void GetWorldTransforms(Matrix4f* xform) const		{ xform[0] = mWorldMatrix; }
	uint32_t GetWorldTransformsCount() const					{ return 1; }

	void SetWorldMatrix(const Matrix4f& mat)				{ mWorldMatrix = mat; }

protected:
	String mName;
	BoundingSpheref mBoundingSphere;

	Matrix4f mWorldMatrix;
	shared_ptr<Material> mMaterial;
};

class _ApiExport SimpleBox : public RenderableHelper
{
public:
	SimpleBox(const String& name);
	~SimpleBox();

	const shared_ptr<RenderOperation>& GetRenderOperation() const;
};

class _ApiExport SimpleTexturedQuad : public RenderableHelper
{
public:
	SimpleTexturedQuad(const String& name);
	~SimpleTexturedQuad();

	const shared_ptr<RenderOperation>& GetRenderOperation() const;
};

} // Namespace RcEngine

#endif // SimpleGeometry_h__

