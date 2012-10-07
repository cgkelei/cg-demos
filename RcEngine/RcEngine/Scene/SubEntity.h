#ifndef SubEntity_h__
#define SubEntity_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>

namespace RcEngine {

class _ApiExport SubEntity : public Renderable
{
public:
	SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart );
	~SubEntity();

	const BoundingSpheref& GetBoundingSphere() const;

	const String& GetName() const ;

	const shared_ptr<Material>& GetMaterial() const;

	void SetMaterial( const shared_ptr<Material>& mat );
	void SetMaterial( const String& matName, const String& group );

	EffectTechnique* GetTechnique() const; 

	const shared_ptr<RenderOperation>& GetRenderOperation() const;

	void GetWorldTransforms(Matrix4f* xform) const;
	uint32_t GetWorldTransformsCount() const;

protected:
	Entity* mParent;
	shared_ptr<MeshPart> mMeshPart;
	shared_ptr<RenderOperation> mRenderOperation;
	shared_ptr<Material> mMaterial;
};



}

#endif // SubEntity_h__
