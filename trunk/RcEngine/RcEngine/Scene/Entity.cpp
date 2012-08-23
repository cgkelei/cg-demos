#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <Graphics/Mesh.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Entity::Entity( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name)
{

}

Entity::~Entity()
{

}

const BoundingSpheref& Entity::GetWorldBoundingSphere() const
{
	if (!mParent)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " Haven't attach to a scene node yet, world bound not exits",
			"Entity::GetWorldBoundingSphere");
	}

	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	mBoundingShere = Transform(mMesh->GetBoundingSphere(), mParent->GetWorldTransform());

	return mBoundingShere;
}



//------------------------------------------------------------------------------------------------------
SubEntity::SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart )
	: mMeshPart(meshPart), mParent(parent)
{

}

SubEntity::~SubEntity()
{

}

const shared_ptr<Material>& SubEntity::GetMaterial() const
{
	uint32_t materialIndex = mMeshPart->GetMaterialID();
	return mParent->GetMesh()->GetMaterial(materialIndex);
}

void SubEntity::SetMaterial( const shared_ptr<Material>& mat )
{
	uint32_t materialIndex = mMeshPart->GetMaterialID();
	mParent->GetMesh()->SetMaterial(materialIndex, mat);
}




}