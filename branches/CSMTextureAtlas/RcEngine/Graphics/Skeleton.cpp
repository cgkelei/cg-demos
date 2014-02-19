#include <Graphics/Skeleton.h>
#include <IO/FileStream.h>
#include <Core/XMLDom.h>
#include <Scene/Entity.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

Bone::Bone( const String& name, uint32_t boneID, Bone* parent )
	: Node(name, parent), mBoneIndex(boneID)
{

}

void Bone::CalculateBindPose()
{
	float4x4 bindPose = GetWorldTransform();
	mOffsetMatrix = bindPose.Inverse();
}

Node* Bone::CreateChildImpl( const String& name )
{
	ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Shound't call Bone::CreateChildImpl", "Bone::CreateChildImpl");
	return nullptr;
}

Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
	// Delete all bones
	for (Bone* bone : mBones)
		delete bone;
	
	for (BoneFollower* follower : mFollowers)
		delete follower;
}

Bone* Skeleton::GetRootBone()
{
	return mBones[0];
}

Bone* Skeleton::GetBone( const String& name )
{
	for (Bone* bone : mBones)
	{
		if (bone->GetName() == name)
			return bone;
	}

	return nullptr;
}

Bone* Skeleton::GetBone( uint32_t index )
{
	return mBones[index];
}

shared_ptr<Skeleton> Skeleton::LoadFrom( Stream& source )
{
	uint32_t numBones = source.ReadUInt();
	
	// no skeleton info
	if (numBones == 0)
		return nullptr;

	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();
	skeleton->mBones.resize(numBones);

	for (uint32_t i = 0; i < numBones; ++i)
	{
		String boneName = source.ReadString();
		String parentName = source.ReadString();

		Bone* parent = parentName.empty() ? nullptr : skeleton->GetBone(parentName);
		Bone* bone = new Bone(boneName, i, parent);

		float3 bindPos;
		source.Read(&bindPos,sizeof(float3));
		bone->SetPosition(bindPos);

		Quaternionf bindRot;
		source.Read(&bindRot, sizeof(Quaternionf));
		bone->SetRotation(bindRot);

		float3 bindScale;
		source.Read(&bindScale,sizeof(float3));
		bone->SetScale(bindScale);

		bone->CalculateBindPose();

		skeleton->mBones[i] = bone;	
	}

	return skeleton;
}

shared_ptr<Skeleton> Skeleton::Clone()
{
	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

	skeleton->mBones.resize(mBones.size());
	for (size_t iBone = 0; iBone < mBones.size(); ++iBone)
	{
		Bone* parentBone = (iBone == 0) ? nullptr : skeleton->mBones[(static_cast<Bone*>(mBones[iBone]->GetParent()))->GetBoneIndex()];
		Bone* newBone =  new Bone( mBones[iBone]->GetName(), iBone,  parentBone);
		newBone->SetPosition(mBones[iBone]->GetPosition());
		newBone->SetRotation(mBones[iBone]->GetRotation());
		newBone->SetScale(mBones[iBone]->GetScale());
		newBone->CalculateBindPose();
		skeleton->mBones[iBone] = newBone;
	}

	return skeleton;
}

Bone* Skeleton::AddBone( const String& name, Bone* parent )
{
	Bone* bone = new Bone(name, mBones.size(), parent);
	mBones.push_back(bone);
	return bone;
}

BoneFollower* Skeleton::CreateFollowerOnBone( Bone* bone, const Quaternionf &offsetOrientation /*= Quaternionf::Identity()*/, const float3 &offsetPosition /*= float3::Zero()*/ )
{
	BoneFollower* follower = new BoneFollower(bone);

	follower->SetPosition(offsetPosition);
	follower->SetRotation(offsetOrientation);
	follower->SetScale(float3(1, 1, 1));
	
	return follower;
}

void Skeleton::FreeFollower( BoneFollower* follower )
{
	assert(follower != nullptr);

	for (auto it = mFollowers.begin(); it != mFollowers.end(); ++it)
	{
		if ( (*it) == follower )
		{
			if (follower->GetParent())
			{
				follower->GetParent()->DetachChild(follower);
			}

			mFollowers.erase(it);
			break;
		}
	}

	delete follower;
}

//------------------------------------------------------------------------------
BoneFollower::BoneFollower(Bone* bone)
	: Bone(bone->GetName(), -1, bone),
	  mParentEntity(nullptr),
	  mFollower(nullptr)
{

}

BoneFollower::~BoneFollower()
{

}

void BoneFollower::SetParentEntity( Entity* entity )
{
	mParentEntity = entity;
}

void BoneFollower::SetFollower( SceneObject* follower )
{
	mFollower = follower;
	mFollower->OnAttach(this);
}

void BoneFollower::UpdateWorldTransform() const
{
	Bone::UpdateWorldTransform();

	if (mParentEntity)
	{
		Node* entityParentNode = mParentEntity->GetParentNode();
		if (entityParentNode)
			mWorldTransform = mWorldTransform * entityParentNode->GetWorldTransform();
	}
}

} // Namespace RcEngine
