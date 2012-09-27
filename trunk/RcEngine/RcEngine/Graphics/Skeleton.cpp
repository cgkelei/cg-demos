#include <Graphics/Skeleton.h>
#include <IO/FileStream.h>
#include <Core/XMLDom.h>

namespace RcEngine {

Bone::Bone( const String& name, uint32_t boneID, Bone* parent )
	: Node(name, parent), mBoneIndex(boneID)
{

}

Matrix4f Bone::GetOffsetMatrix() const
{
	return Matrix4f();
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

		bone->mRadius = source.ReadFloat();

		skeleton->mBones[i] = bone;	
	}

	return skeleton;
}

shared_ptr<Skeleton> Skeleton::LoadFrom( Stream& source, int a )
{
	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();
	
	int i = 0;
	for (XMLNodePtr node = root->FirstNode("Bone"); node; node = node->NextSibling("Bone"))
	{
		String name = node->AttributeString("name", "");
		int parentIndex = node->AttributeInt("parent", -1);

		Vector3f pos, scale;
		Quaternionf rot;

		pos.X() = node->FirstNode("BindPosition")->AttributeFloat("x", 0);
		pos.Y() = node->FirstNode("BindPosition")->AttributeFloat("y", 0);
		pos.Z() = node->FirstNode("BindPosition")->AttributeFloat("z", 0);

		scale.X() = node->FirstNode("BindScale")->AttributeFloat("x", 0);
		scale.Y() = node->FirstNode("BindScale")->AttributeFloat("y", 0);
		scale.Z() = node->FirstNode("BindScale")->AttributeFloat("z", 0);

		rot.X() = node->FirstNode("BindRotation")->AttributeFloat("x", 0);
		rot.Y() = node->FirstNode("BindRotation")->AttributeFloat("y", 0);
		rot.Z() = node->FirstNode("BindRotation")->AttributeFloat("z", 0);
		rot.W() = node->FirstNode("BindRotation")->AttributeFloat("w", 0);

		Bone* parent;
		if (parentIndex == -1)
		{
			parent = nullptr;
		}
		else
		{
			parent = skeleton->GetBone(parentIndex);
		}

		Bone* bone = new Bone(name, i++, parent);

		
		bone->SetPosition(pos);
		bone->SetRotation(rot);
		bone->SetScale(scale);

		bone->mRadius = 0;

		skeleton->mBones.push_back(bone);		
	}

	return skeleton;
}

} // Namespace RcEngine
