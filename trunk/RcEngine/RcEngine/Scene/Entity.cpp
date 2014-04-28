#include <Scene/Entity.h>
#include <Scene/SubEntity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshPart.h>
#include <Graphics/Effect.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Graphics/RenderQueue.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <IO/PathUtil.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

Entity::Entity( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name, SOT_Entity, true),
	mNumSkinMatrices(0), 
	mMesh(mesh), 
	mAnimationPlayer(nullptr),
	mSkeleton( mesh->GetSkeleton() ? mesh->GetSkeleton()->Clone() : 0 )
{
	Initialize();

	printf("Create Entity: %s\n", name.c_str());
}

Entity::~Entity()
{
	printf("Delete Entity: %s\n", mName.c_str());

	for (SubEntity* subEntiry : mSubEntityList)
		SAFE_DELETE(subEntiry);
	mSubEntityList.clear();

	mChildAttachedObjects.clear();

	SAFE_DELETE(mAnimationPlayer);
}


void Entity::Initialize()
{
	const String& meshGroup = mMesh->GetResourceGroup();
	String meshDirectory = PathUtil::GetPath(mMesh->GetResourceName()); 

	for (uint32_t i = 0; i < mMesh->GetNumMeshPart(); ++i)
	{
		shared_ptr<MeshPart> meshPart = mMesh->GetMeshPart(i);
		
		String matResName = meshDirectory + meshPart->GetMaterialName();

		shared_ptr<Material> material = std::static_pointer_cast<Material>(
			ResourceManager::GetSingleton().GetResourceByName(RT_Material, matResName, meshGroup));
		material->Load();

		SubEntity* subEnt = new SubEntity(this, meshPart);
		subEnt->SetMaterial(material);

		mSubEntityList.push_back( subEnt );
	}

	if (HasSkeleton())
	{
		mNumSkinMatrices = mSkeleton->GetBoneCount();
		mSkinMatrices.resize(mNumSkinMatrices);
	}

	if (mMesh->HasAnimation())
	{
		mAnimationPlayer = new SkinnedAnimationPlayer(mSkeleton);

		for (const String& animClipResName : mMesh->GetAnimations())
		{
			String clipName = meshDirectory + animClipResName;		 
			shared_ptr<AnimationClip> clip = std::static_pointer_cast<AnimationClip>(
				ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, meshGroup));
			clip->Load();

			mAnimationPlayer->AddClip(clip);
		}	
	}

	if (mParentNode)
	{
		mParentNode->NeedUpdate();
	}
}

void Entity::OnAttach( Node* node )
{
	SceneObject::OnAttach(node);
}

void Entity::OnDetach( Node* node )
{
	SceneObject::OnDetach(node);
}

const BoundingBoxf& Entity::GetWorldBoundingBox() const
{
	if (!mParentNode)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " Haven't attach to a scene node yet, world bound not exits",
			"Entity::GetWorldBoundingSphere");
	}

	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	mWorldBoundingBox = Transform(mMesh->GetBoundingBox(), mParentNode->GetWorldTransform());

	return mWorldBoundingBox;
}

const BoundingBoxf& Entity::GetLocalBoundingBox() const
{
	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	return mMesh->GetBoundingBox();
}

bool Entity::HasSkeleton() const
{
	return mSkeleton != nullptr;
}

shared_ptr<Skeleton> Entity::GetSkeleton()
{
	return mSkeleton;
}

bool Entity::HasSkeletonAnimation() const
{
	return mMesh->HasAnimation();
}

AnimationPlayer* Entity::GetAnimationPlayer() const
{
	return mAnimationPlayer;
}

void Entity::OnUpdateRenderQueue(RenderQueue* renderQueue, const Camera& camera, RenderOrder order)
{
	// Add each visible SubEntity to the queue
	for (SubEntity* subEntity : mSubEntityList)
	{
		BoundingBoxf subWorldBoud = Transform(subEntity->GetBoundingBox(), mParentNode->GetWorldTransform());

		// tode  mesh part world bounding has some bugs.
		if(camera.Visible(subWorldBoud))
		{
			float sortKey = 0;
			RenderQueue::Bucket bucket = (RenderQueue::Bucket)subEntity->GetMaterial()->GetQueueBucket();

			switch( order )
			{
			case RO_StateChange:
				sortKey = (float)subEntity->GetMaterial()->GetEffect()->GetResourceHandle();
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

			renderQueue->AddToQueue(RenderQueueItem(subEntity, sortKey), bucket);			
		}
	}

	// Update animation 
	if (HasSkeleton())
	{
		UpdateAnimation();

		for (auto& kv : mChildAttachedObjects)
		{
			SceneObject* child = kv.second;

			bool renderable = child->Renderable();
			bool visible = camera.Visible(child->GetWorldBoundingBox());
			
			/*bool visible = true;*/
			if (visible)
			{
				//Check if the bone exists in the current LOD
				//The child is connected to a joint
				Bone* bone = static_cast<Bone*>(child->GetParentNode());

				if (bone && !GetSkeleton()->GetBone( bone->GetName()) )
				{
					//Current LOD entity does not have the bone that the
					//child is connected to. Do not display.
					visible = false;
				}
			}

			if (visible && renderable)
			{
				child->OnUpdateRenderQueue(renderQueue, camera, order);
			}   
		}
	}

	if (mDisplaySkeleton && HasSkeleton())
	{
		
		uint32_t numBones = mSkeleton->GetBoneCount();
		for (uint32_t b = 0; b < numBones; ++b)
		{
			Bone* bone = mSkeleton->GetBone(b);
			/*if (mRenderQueuePrioritySet)
			{
				assert(mRenderQueueIDSet == true);
				queue->addRenderable(bone->getDebugRenderable(1), mRenderQueueID, mRenderQueuePriority);
			}
			else if(mRenderQueueIDSet)
			{
				queue->addRenderable(bone->getDebugRenderable(1), mRenderQueueID);
			} 
			else 
			{
				queue->addRenderable(bone->getDebugRenderable(1));
			}*/
		}
	}
}

void Entity::UpdateAnimation()
{
	for (auto& kv : mAnimationPlayer->GetAllClip())
	{
		AnimationState* animState = kv.second;
		animState->Apply();
	}
	
	// Note: the model's world transform will be baked in the skin matrices
	const vector<Bone*>& bones = mSkeleton->GetBones();
	for (size_t i = 0; i < bones.size(); ++i)
	{
		Bone* bone = bones[i];
		mSkinMatrices[i] = bone->GetOffsetMatrix() * bone->GetWorldTransform();
	}
}

BoneFollower* Entity::AttachObjectToBone( const String &boneName, SceneObject* sceneObj, const Quaternionf& offsetOrientation/*= Quaternionf::Identity()*/, const float3 & offsetPosition /*= float3::Zero()*/ )
{
	if (!HasSkeleton())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "This entity's mesh has no skeleton to attach object to.",
			"Entity::attachObjectToBone");
	}

	if (mChildAttachedObjects.find(sceneObj->GetName()) != mChildAttachedObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
			"An object with the name " + sceneObj->GetName() + " already attached",
			"Entity::AttachObjectToBone");
	}

	if(sceneObj->IsAttached())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Object already attached to a sceneNode or a Bone",
			"Entity::attachObjectToBone");
	}

	Bone* pBone = mSkeleton->GetBone(boneName);

	if (!pBone)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Cannot locate bone named " + boneName,
			"Entity::attachObjectToBone");
	}

	BoneFollower* pBoneFollower = mSkeleton->CreateFollowerOnBone(pBone, offsetOrientation, offsetPosition);	
	pBoneFollower->SetParentEntity(this);
	pBoneFollower->SetFollower(sceneObj);
	
	mChildAttachedObjects.insert( std::make_pair(sceneObj->GetName(), sceneObj) );

	// Trigger update of bounding box if necessary
	if (mParentNode)
		mParentNode->NeedUpdate();

	return pBoneFollower;
}


SceneObject* Entity::FactoryFunc( const String& name, const NameValuePairList* params)
{
	// must have mesh parameter
	shared_ptr<Mesh> pMesh;

	if (params)
	{
		String groupName = "General";

		NameValuePairList::const_iterator found = params->find("ResourceGroup");
		if (found != params->end())
			groupName = found->second;

		found = params->find("Mesh");
		if (found != params->end())
		{
			// Get mesh (load if required)
			pMesh = std::static_pointer_cast<Mesh>(
				ResourceManager::GetSingleton().GetResourceByName(RT_Mesh, found->second, groupName));	
		}
	}
	else
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Create entity failed.", "Entity::FactoryFunc");

	return new Entity(name, pMesh);
}





}