#include <Scene/Entity.h>
#include <Scene/SubEntity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshPart.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

Entity::Entity( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name), mNumBoneMatrices(0), mMesh(mesh),
	mSkeleton( mesh->GetSkeleton() ? mesh->GetSkeleton()->Clone() : 0 )
{
	Initialize();
}

Entity::~Entity()
{

}

const BoundingSpheref& Entity::GetWorldBoundingSphere() const
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

	mBoundingShere = Transform(mMesh->GetBoundingSphere(), mParentNode->GetWorldTransform());

	return mBoundingShere;
}

void Entity::Initialize()
{
	for (uint32_t i = 0; i < mMesh->GetNumMeshPart(); ++i)
	{
		shared_ptr<MeshPart> meshPart = mMesh->GetMeshPart(i);
		
		SubEntity* subEnt = new SubEntity(this, meshPart);
		subEnt->SetMaterial(meshPart->GetMaterialName(), mMesh->GetResourceGroup());

		mSubEntityList.push_back( subEnt );
	}

	if (HasSkeleton())
	{
		mNumBoneMatrices = mSkeleton->GetBoneCount();
		mSkinMatrices.resize(mNumBoneMatrices);
	}


	if (mMesh->HasAnimation())
	{
		mAnimationPlayer = new SkinnedAnimationPlayer(mSkeleton);

		const vector<String>& animations = mMesh->GetAnimations();
		
		for (size_t i = 0; i != animations.size(); ++i)
		{
			shared_ptr<AnimationClip> clip = std::static_pointer_cast<AnimationClip>(
				ResourceManager::GetSingleton().GetResourceByName(ResourceTypes::Animation, animations[i], mMesh->GetResourceGroup()));
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

bool Entity::HasSkeleton() const
{
	return mSkeleton != nullptr;
}

shared_ptr<Skeleton> Entity::GetSkeleton()
{
	return mSkeleton;
}

bool Entity::HasAnimation() const
{
	return mMesh->HasAnimation();
}

AnimationPlayer* Entity::GetAnimationPlayer() const
{
	return mAnimationPlayer;
}

void Entity::UpdateRenderQueue( RenderQueue& renderQueue, Camera* cam )
{
	// Add each visible SubEntity to the queue
	for (auto iter= mSubEntityList.begin(); iter != mSubEntityList.end(); ++iter)
	{
		SubEntity* subEntity = *iter;
		BoundingSpheref subWorldBoud = Transform(subEntity->GetBoundingSphere(), mParentNode->GetWorldTransform());
		
		if(cam->Visible(subWorldBoud))
		{
			RenderQueueItem item;
			item.Renderable = subEntity;
			item.Type = SOT_Entity;

			// todo add render queue sort
			item.SortKey = 0.0f;

			renderQueue.push_back(item);
		}
	}

	// Update animation 
	if (HasSkeleton())
	{
		UpdateAnimation();

		for(auto iter = mChildAttachedObjects.begin(); iter != mChildAttachedObjects.end(); ++iter)
		{
			SceneObject* child = iter->second;

			bool visible = cam->Visible(child->GetWorldBoundingSphere());

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

			if (visible)
			{
				child->UpdateRenderQueue(renderQueue, cam);
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
	const unordered_map<String, AnimationState*>& clipStates = mAnimationPlayer->GetAllClip();
	for (auto iter = clipStates.begin(); iter != clipStates.end(); ++iter)
	{
		AnimationState* animState = iter->second;

		animState->Apply();
	}
		
	// Note: the model's world transform will be baked in the skin matrices
	const vector<Bone*>& bones = mSkeleton->GetBones();
		
	if (mBoneWorldMatrices.empty())
	{
		mBoneWorldMatrices.resize(bones.size());
	}

	for (size_t i = 0; i < bones.size(); ++i)
	{
		Bone* bone = bones[i];
		mBoneWorldMatrices[i] = bone->GetOffsetMatrix() * bone->GetWorldTransform() * GetWorldTransform();
	}
}

Bone* Entity::AttachObjectToBone( const String &boneName, SceneObject* sceneObj, const Quaternionf& offsetOrientation/*= Quaternionf::Identity()*/, const Vector3f & offsetPosition /*= Vector3f::Zero()*/ )
{
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

	if (!HasSkeleton())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "This entity's mesh has no skeleton to attach object to.",
			"Entity::attachObjectToBone");
	}

	Bone* bone = mSkeleton->GetBone(boneName);

	if (!bone)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Cannot locate bone named " + boneName,
			"Entity::attachObjectToBone");
	}
	
	/*sceneObj->OnAttach(bone);
	mChildAttachedObjects.insert( std::make_pair(sceneObj->GetName(), sceneObj));*/

	// Trigger update of bounding box if necessary
	/*if (mParentNode)
		mParentNode->NeedUpdate();
*/
	return bone;
}



}