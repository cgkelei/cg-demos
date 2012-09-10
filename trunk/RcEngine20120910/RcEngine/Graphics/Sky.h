#ifndef Sky_h__
#define Sky_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>

namespace RcEngine {

class SkyPlane;
class SkyBoxCube;

class _ApiExport SkyBox : public SceneObject
{
public:
	enum SkyBoxPlane
	{
		Front = 0,
		Back,
		Left,
		Right,
		Up,
		Down,
		MaxPlaneCount
	};

public:
	SkyBox( float size = 100.0f,  bool cube = false );
	~SkyBox();

	SceneObejctType GetSceneObjectType() const  { return SOT_Sky; }

	bool Renderable() const	{ return true; }

	/**
	 * Set each sky plane material.
	 */
	void SetMaterial( SkyBoxPlane plane, const shared_ptr<Material>& mat );
	
	/**
	 * Set cubic sky box material.
	 */
	void SetMaterial( const shared_ptr<Material>& mat );

	void AddToRenderQueue( RenderQueue& renderQueue ) const;

private:
	bool mCubeMapSky;
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Material> mMaterials[MaxPlaneCount];
	shared_ptr<Material> mMaterialCubeMap;

	SkyPlane* mSkyPlanes[MaxPlaneCount];
	SkyBoxCube* mSkyCubeBox;

	friend class SkyPlane;
	friend class SkyBoxCube;
};


}

#endif // Sky_h__
