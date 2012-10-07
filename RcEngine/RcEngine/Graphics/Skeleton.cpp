#include <Graphics/Skeleton.h>
#include <IO/FileStream.h>
#include <Core/XMLDom.h>
#include <Scene/Entity.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Bone::Bone( const String& name, uint32_t boneID, Bone* parent )
	: Node(name, parent), mBoneIndex(boneID)
{

}


void Bone::CalculateBindPose()
{
	Matrix4f bindPose = GetWorldTransform();
	mOffsetMatrix = bindPose.Inverse();
}

//void Bone::SetParentEntity( Entity* ent )
//{
//	// a bone can only belong to on entity
//	if (mParentEntity != nullptr)
//	{
//		assert(mParentEntity == ent);
//	}
//}
//
//Entity* Bone::GetParentParentEntity() const
//{
//	return mParentEntity;
//}


Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}

Bone* Skeleton::GetRootBone()
{
	return mBones[0];
}

Bone* Skeleton::GetBone( const String& name )
{
	auto found = std::find_if(mBones.begin(), mBones.end(), [&](Bone* bone)
					{
						return name == bone->GetName();
						
					});

	if (found == mBones.end())
		return nullptr;

	return *found;
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

		Bone* parent;
		if (parentName.empty())
		{
			parent = nullptr;
		}
		else
		{
			parent = skeleton->GetBone(parentName);
		}

		Bone* bone = new Bone(boneName, i, parent);

		Vector3f bindPos;
		source.Read(&bindPos,sizeof(Vector3f));
		bone->SetPosition(bindPos);

		Quaternionf bindRot;
		source.Read(&bindRot, sizeof(Quaternionf));
		bone->SetRotation(bindRot);

		Vector3f bindScale;
		source.Read(&bindScale,sizeof(Vector3f));
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

} // Namespace RcEngine
