#include <Scene/SubEntity.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshPart.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

SubEntity::SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart )
	: mMeshPart(meshPart), mParent(parent), mRenderOperation(new RenderOperation)
{

}

SubEntity::~SubEntity()
{

}

const shared_ptr<Material>& SubEntity::GetMaterial() const
{
	return mMaterial;
}

void SubEntity::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterial = mat;
	mMaterial->Load();
}

void SubEntity::SetMaterial( const String& matName, const String& group )
{
	mMaterial = std::static_pointer_cast<Material>(
		ResourceManager::GetSingleton().GetResourceByName(ResourceTypes::Material, matName, group));

	mMaterial->Load();
}

const String& SubEntity::GetName() const
{
	return mMeshPart->GetName();
}

EffectTechnique* SubEntity::GetTechnique() const
{
	return mMaterial->GetCurrentTechnique();
}

void SubEntity::GetWorldTransforms( Matrix4f* xform ) const
{
	if (!mParent->mNumBoneMatrices || !mParent->HasAnimation())
	{
		// no skeleton animation
		*xform = mParent->GetWorldTransform();
	}
	else
	{
		for (size_t i = 0; i < mParent->mNumBoneMatrices; ++i)
		{
			xform[i] = mParent->mBoneWorldMatrices[i];
		}
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
		return mParent->mNumBoneMatrices;
	}
}

const shared_ptr<RenderOperation>& SubEntity::GetRenderOperation() const
{
	mMeshPart->GetRenderOperation(*mRenderOperation, 0);
	return mRenderOperation;
}

const BoundingBoxf& SubEntity::GetBoundingBox() const
{
	return mMeshPart->GetBoundingBox();
}


}