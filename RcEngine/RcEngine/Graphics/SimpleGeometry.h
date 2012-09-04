#ifndef SimpleGeometry_h__
#define SimpleGeometry_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Scene/SceneObject.h>
#include <Math/BoundingSphere.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport RenderableHelper : public Renderable, public SceneObject
{
public:
	RenderableHelper(const String& name);
	virtual ~RenderableHelper();

	// overload from scene object
	virtual SceneObejctType GetSceneObjectType() const			  { return SOT_Entity; }
	
	// overload from renderable
	const shared_ptr<RenderOperation>& GetRenderOperation() const { return mRenderOperation; }
	const shared_ptr<Material>& GetMaterial() const				  { return mMaterial; }
	uint32_t GetWorldTransformsCount() const					  { return 1; }
	void GetWorldTransforms(Matrix4f* xform) const;

	void SetMaterial( const shared_ptr<Material>& mat )			{ mMaterial = mat; }

protected:
	shared_ptr<Material> mMaterial;
};

class _ApiExport SimpleBox : public RenderableHelper
{
public:
	SimpleBox(const String& name);
	~SimpleBox();

protected:
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
};

class _ApiExport SimpleTexturedQuad : public RenderableHelper
{
public:
	SimpleTexturedQuad(const String& name);
	~SimpleTexturedQuad();

protected:
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
};

} // Namespace RcEngine

#endif // SimpleGeometry_h__

