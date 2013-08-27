#ifndef Sky_h__
#define Sky_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Renderable.h>

namespace RcEngine {

class _ApiExport Sky : public SceneObject, public Renderable
{
public:
	enum SkyBoxPlane
	{
		Left = 0,
		Right,
		Up,
		Down,
		Front,
		Back,
		MaxPlaneCount
	};

public:
	Sky( float size,  bool cube = false );
	~Sky();

	SceneObejctType GetSceneObjectType() const  { return SOT_Sky; }

	bool Renderable() const	{ return true; }

	const shared_ptr<Material>& GetMaterial() const	{ return mMaterial; }

	const shared_ptr<RenderOperation>& GetRenderOperation() const	{ return mRenderOperation; }

	uint32_t GetWorldTransformsCount() const;
	void GetWorldTransforms(float4x4* xform) const;
	
	void SetMaterial( const shared_ptr<Material>& mat );

private:
	void InitializeSkyBox(float size);
	void InitializeSkyCubeMap(float size);

private:
	bool mCubeMapSky;
	
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Material> mMaterial;

	shared_ptr<RenderOperation> mRenderOperation;
};


}

#endif // Sky_h__
