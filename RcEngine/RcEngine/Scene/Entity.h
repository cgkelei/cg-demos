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
public:
	Entity( const String& name, const shared_ptr<Mesh>& mesh );
	~Entity();

	SceneObejctType GetSceneObjectType() const	{ return SOT_Entity; }
	const BoundingSpheref& GetWorldBoundingSphere() const;

	const shared_ptr<Mesh>& GetMesh() const { return mMesh; }
	
	/**
	 * Sets the material to use for the whole of this entity.
	 */
	void SetMaterial( const shared_ptr<Material>& mat );

	bool HasSkeleton() const	{ return false; }


protected:
	shared_ptr<Mesh> mMesh;
	mutable BoundingSpheref mBoundingShere;

};

class SubEntity : public Renderable
{
public:
	SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart );
	~SubEntity();

	const shared_ptr<Material>& GetMaterial() const;
	void SetMaterial( const shared_ptr<Material>& mat );

protected:
	Entity* mParent;
	shared_ptr<MeshPart> mMeshPart;

};

}


#endif // Entity_h__
