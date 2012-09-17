#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Entity::Entity( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name), mNumBoneMatrices(0), mMesh(mesh)
{
	Initialize();
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

void Entity::Initialize()
{
	SubEntity* subEnt = nullptr;
	for (uint32_t i = 0; i < mMesh->GetNumMeshPart(); ++i)
	{
		auto subMesh = mMesh->GetMeshPart(i);
		mSubEntityList.push_back( new SubEntity(this, subMesh) );
	}
}

void Entity::OnAttach( SceneNode* node )
{
	SceneObject::OnAttach(node);
}

void Entity::OnDetach( SceneNode* node )
{

}


const BoundingSpheref& Entity::GetBoundingSphere() const
{
	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	return mMesh->GetBoundingSphere();
}

void Entity::SetMaterial( const shared_ptr<Material>& mat )
{
	mMesh->SetMaterial(mat);
}

//------------------------------------------------------------------------------------------------------
SubEntity::SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart )
	: mMeshPart(meshPart), mParent(parent), mRenderOperation(new RenderOperation)
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

const String& SubEntity::GetName() const
{
	return mMeshPart->GetName();
}

EffectTechnique* SubEntity::GetTechnique() const
{
	return mMeshPart->GetMaterial()->GetCurrentTechnique();
}

void SubEntity::GetWorldTransforms( Matrix4f* xform ) const
{
	if (!mParent->mNumBoneMatrices)
	{
		*xform = mParent->GetWorldTransform();
	}
}

std::uint32_t SubEntity::GetWorldTransformsCount() const
{
	if (!mParent->mNumBoneMatrices)
	{
		// No Skin animation
		return 1;
	}
	else
	{
		// to do,add skin mesh support
		return 2;
	}
}

const shared_ptr<RenderOperation>& SubEntity::GetRenderOperation() const
{
	mMeshPart->GetRenderOperation(*mRenderOperation, 0);
	return mRenderOperation;
}




}