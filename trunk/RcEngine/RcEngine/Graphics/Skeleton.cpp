#include <Graphics/Skeleton.h>
#include <IO/FileStream.h>
#include <Core/XMLDom.h>

namespace RcEngine {

Bone::Bone( const String& name, uint32_t boneID, Bone* parent )
	: Node(name, parent), mBoneIndex(boneID)
{

}


Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}

Bone* Skeleton::GetRootBone()
{
	if (mRootBoneIndex == -1)
		return nullptr;

	return mBones[mRootBoneIndex];
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
		uint32_t parentIndex = source.ReadUInt();

		Bone* parent;
		if (parentIndex != -1)
		{
			parent = skeleton->GetBone(parentIndex);
		}
		else
		{
			parent = nullptr;
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

		skeleton->mBones[i] = bone;	
	}

	return skeleton;
}

} // Namespace RcEngine
