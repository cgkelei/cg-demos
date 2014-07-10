#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>
#include <Scene/SceneNode.h>

namespace RcEngine {

class Bone;
class BoneSceneNode;
class SceneObject;
class Visual;

class _ApiExport Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void Update( float delta );

	uint32_t GetNumBones() const { return mBones.size(); }

	Bone* GetRootBone();
	Bone* GetBone( const String& name );
	Bone* GetBone( uint32_t index );

	Bone* AddBone(const String& name, Bone* parent);
	
	inline uint32_t GetNumBoneSceneNodes() const { return mBoneSceneNodes.size(); }
	inline BoneSceneNode* GetBoneSceneNode(uint32_t i) const { return mBoneSceneNodes[i]; }

	BoneSceneNode* CreateBoneSceneNode(const String& nodeName, const String& boneName, SceneNode* worldSceneNode);

	shared_ptr<Skeleton> Clone();

public:
	static shared_ptr<Skeleton> LoadFrom( Stream& source );

private:
	std::vector<Bone*> mBones;
	std::vector<BoneSceneNode*> mBoneSceneNodes;
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

class _ApiExport BoneSceneNode : public SceneNode
{
public:
	BoneSceneNode(const String& name, SceneNode* worldSceneNode);

protected:
	// need to consider entity's transform
	virtual void UpdateWorldTransform() const;

	virtual Node* CreateChildImpl(const String& name);

protected:
	SceneNode* mWorldSceneNode;
};

} // Namespace RcEngine

#endif // Skeleton_h__