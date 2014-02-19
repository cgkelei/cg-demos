#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>

namespace RcEngine {

class Bone;
class BoneFollower;
class SceneObject;
class Entity;

class _ApiExport Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void Update( float delta );

	uint32_t GetBoneCount() const { return mBones.size(); }

	Bone* GetRootBone();
	Bone* GetBone( const String& name );
	Bone* GetBone( uint32_t index );

	Bone* AddBone(const String& name, Bone* parent);

	const vector<Bone*>& GetBones() const { return mBones; }
	vector<Bone*>& GetBonesModified() { return mBones; }

	BoneFollower* CreateFollowerOnBone(Bone* bone, const Quaternionf &offsetOrientation = Quaternionf::Identity(), 
		const float3 &offsetPosition = float3::Zero());

	void FreeFollower(BoneFollower* follower);

	shared_ptr<Skeleton> Clone();

public:
	static shared_ptr<Skeleton> LoadFrom( Stream& source );

private:
	std::vector<Bone*> mBones;
	std::list<BoneFollower*> mFollowers;
};

class _ApiExport Bone : public Node
{
public:
	Bone(const String& name, uint32_t boneIndex, Bone* parent = 0);

	uint32_t GetBoneIndex() const { return mBoneIndex; }

	const float4x4& GetOffsetMatrix() const { return mOffsetMatrix; }

	void CalculateBindPose();	

protected:

	// Do not use this method to create bone node, Bone node only created when load from mesh file
	virtual Node* CreateChildImpl(const String& name);

protected:
	uint32_t mBoneIndex;
	float4x4 mOffsetMatrix;
};

/**
 * A follower point on a skeleton, which can be used to attach 
 * SceneObjects to on specific other entities.
 */
class _ApiExport BoneFollower : public Bone 
{
public:
	BoneFollower(Bone* bone);
	virtual ~BoneFollower();

	Entity* GetParentEntity() const  { return mParentEntity; }
	SceneObject* GetFollower() const { return mFollower; }
	
	void SetParentEntity(Entity* entity);
	void SetFollower(SceneObject* follower);

protected:
	// need to consider entity's transform
	virtual void UpdateWorldTransform() const;

protected:
	Entity* mParentEntity;
	SceneObject* mFollower;


};



} // Namespace RcEngine

#endif // Skeleton_h__