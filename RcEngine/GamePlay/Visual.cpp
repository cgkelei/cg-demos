#include <GamePlay/Visual.h>
#include <Graphics/Mesh.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <GamePlay/GameObject.h>
#include <Scene/SceneNode.h>
#include <Resource/ResourceManager.h>
#include <IO/PathUtil.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

const String Visual::ComponentName("Visual");

Visual::Visual( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name, SOT_Entity, true),
	  mMesh(mesh), 
	  mNumSkinMatrices(0), 
	  mMatrixPalette(nullptr),
	  mSkeleton( mesh->GetSkeleton() ? mesh->GetSkeleton()->Clone() : nullptr )
{
	// Create visual parts
	const String& meshGroup = mMesh->GetResourceGroup();
	String meshDirectory = PathUtil::GetPath(mMesh->GetResourceName()); 

	for (uint32_t i = 0; i < mMesh->GetNumMeshPart(); ++i)
	{
		const shared_ptr<MeshPart>& meshPart = mMesh->GetMeshPart(i);

		VisualPart* visualPart = new VisualPart(*this, meshPart);

		String matResName = meshDirectory + meshPart->GetMaterialName();
		visualPart->SetMaterial( ResourceManager::GetSingleton().GetResourceByName<Material>(RT_Material, matResName, meshGroup) );

		mVisualParts.push_back( visualPart );
	}

	if (mMesh->GetSkeleton())
	{
		mNumSkinMatrices = mSkeleton->GetNumBones();
		mMatrixPalette = new float4x4[mNumSkinMatrices];
	}
}

Visual::~Visual()
{
	if (mMatrixPalette)
	{
		delete[] mMatrixPalette;
		mMatrixPalette = nullptr;
	}
}

void Visual::DestroyComponent()
{
	Component::DestroyComponent();
	if (mParentNode)
		mParentNode->DetachOject(this);

}


const BoundingBoxf& Visual::GetWorldBoundingBox() const
{
	assert(mMesh);

	if (!mParentNode)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " Haven't attach to a scene node yet, world bound not exits",
			"Entity::GetWorldBoundingSphere");
	}

	mWorldBoundingBox = Transform(mMesh->GetBoundingBox(), mParentNode->GetWorldTransform());

	return mWorldBoundingBox;
}

void Visual::OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& camera, RenderOrder order )
{
	// Add each visible SubEntity to the queue
	for (VisualPart* visualPart : mVisualParts)
	{
		BoundingBoxf subWorldBoud = Transform(visualPart->GetBoundingBox(), mParentNode->GetWorldTransform());

		// tode  mesh part world bounding has some bugs.
		if(camera.Visible(subWorldBoud))
		{
			float sortKey = 0;
			RenderQueue::Bucket bucket = (RenderQueue::Bucket)visualPart->GetMaterial()->GetQueueBucket();

			switch( order )
			{
			case RO_StateChange:
				sortKey = (float)visualPart->GetMaterial()->GetEffect()->GetResourceHandle();
				break;
			case RO_FrontToBack:
				sortKey = NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
				break;
			case RO_BackToFront:
				sortKey = -NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
				break;
			}

			if (bucket == RenderQueue::BucketTransparent)
			{
				// Transparent object must render from furthest to nearest
				sortKey = -NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
			}

			renderQueue->AddToQueue(RenderQueueItem(visualPart, sortKey), bucket);			
		}
	}

	// Update animation 
	if (mSkeleton)
	{
		UpdateAnimation();

		for (uint32_t i = 0; i < mSkeleton->GetNumBoneSceneNodes(); ++i)
		{
			SceneNode* boneSceneNode = mSkeleton->GetBoneSceneNode(i);
			boneSceneNode->OnUpdateRenderQueues(camera, order);
		}
	}
}

void Visual::UpdateAnimation()
{
	SkinnedAnimationPlayer* animationPlayer = mOwner->GetComponent<SkinnedAnimationPlayer>(SkinnedAnimationPlayer::ComponentName);
	if (animationPlayer)
	{
		for (auto& kv : animationPlayer->GetAllClip())
		{
			AnimationState* animState = kv.second;
			animState->Apply();
		}

		for (size_t i = 0; i < mNumSkinMatrices; ++i)
		{
			Bone* bone = mSkeleton->GetBone(i);
			mMatrixPalette[i] = bone->GetOffsetMatrix() * bone->GetWorldTransform();
		}
	}
}

BoneSceneNode* Visual::CreateBoneSceneNode( const String& nodeName, const String& boneName )
{
	if (!HasSkeleton())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "This entity's mesh has no skeleton to attach object to.",
			"Entity::attachObjectToBone");
	}

	Bone* pBone = mSkeleton->GetBone(boneName);

	if (!pBone)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Cannot locate bone named " + boneName,
			"Entity::attachObjectToBone");
	}

	return mSkeleton->CreateBoneSceneNode(nodeName, boneName, mParentNode);
}

//////////////////////////////////////////////////////////////////////////
VisualPart::VisualPart( Visual& parent, const shared_ptr<MeshPart>& meshPart )
	: mParent(parent)
{

}

const BoundingBoxf& VisualPart::GetBoundingBox() const
{
	return mMeshPart->GetBoundingBox();
}

void VisualPart::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterial = mat;
	if (mMaterial->IsLoaded() == false)
		mMaterial->Load();
}

const shared_ptr<RenderOperation>& VisualPart::GetRenderOperation() const
{
	const uint32_t LOD = 0;

	mMeshPart->GetRenderOperation(*mRenderOperation,LOD);
	return mRenderOperation;
}

void VisualPart::GetWorldTransforms( float4x4* xWorld, uint32_t& numPalette, float4x4*& xPalette )
{
	*xWorld = mParent.GetWorldTransform();

	xPalette = mParent.mMatrixPalette;
	numPalette = mParent.mNumSkinMatrices;
}

}