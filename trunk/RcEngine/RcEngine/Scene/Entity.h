#ifndef Entity_h__
#define Entity_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Renderable.h>


namespace RcEngine {

class SubEntity;

/** 
 * Defines an instance of a discrete, scene object based on a Mesh
 */
class _ApiExport Entity : public SceneObject 
{
	friend class SubEntity;

public:
	Entity( const String& name, const shared_ptr<Mesh>& mesh );
	~Entity();

	virtual bool Renderable() const		{ return true; }

	SceneObejctType GetSceneObjectType() const	{ return SOT_Entity; }

	const BoundingSpheref& GetWorldBoundingSphere() const;

	const BoundingSpheref& GetBoundingSphere() const;

	const shared_ptr<Mesh>& GetMesh() const { return mMesh; }

	uint32_t GetNumSubEntities() const { return mSubEntityList.size(); }

	SubEntity* GetSubEntity( uint32_t index ) const { return mSubEntityList[index]; }
	
	/**
	 * Sets the material to use for the whole of this entity.
	 */
	void SetMaterial( const shared_ptr<Material>& mat );

	bool HasSkeleton() const	{ return false; }

	void OnAttach( SceneNode* node );
	void OnDetach( SceneNode* node );

protected:
	void Initialize();

protected:
	shared_ptr<Mesh> mMesh;
	vector<SubEntity*> mSubEntityList;
	mutable BoundingSpheref mBoundingShere;
	vector<Matrix4f> mBoneMatrices;
	uint32_t mNumBoneMatrices;



};

class _ApiExport SubEntity : public Renderable
{
public:
	SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart );
	~SubEntity();

	const String& GetName() const ;

	const shared_ptr<Material>& GetMaterial() const;
	void SetMaterial( const shared_ptr<Material>& mat );

	EffectTechnique* GetTechnique() const; 

	const shared_ptr<RenderOperation>& GetRenderOperation() const;

	void GetWorldTransforms(Matrix4f* xform) const;
	uint32_t GetWorldTransformsCount() const;

protected:
	Entity* mParent;
	shared_ptr<MeshPart> mMeshPart;
};

}


#endif // Entity_h__
