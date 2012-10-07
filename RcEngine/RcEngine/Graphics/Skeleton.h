#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>

namespace RcEngine {

class Skeleton;

class _ApiExport Bone : public Node
{
public:
	Bone( const String& name, uint32_t boneIndex, Bone* parent = 0 );

	uint32_t GetBoneIndex() const { return mBoneIndex; }

	const Matrix4f& GetOffsetMatrix() const { return mOffsetMatrix; }

	void CalculateBindPose();	

	//void SetParentEntity(Entity* ent);
	//Entity* GetParentParentEntity() const;

protected:
	virtual Node* CreateChildImpl( const String& name ) { return 0; }

private:
	void OnUpdate( );
	
private:
	//Entity* mParentEntity;
	uint32_t mBoneIndex;
	Matrix4f mOffsetMatrix;
};

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

	const vector<Bone*>& GetBones() const { return mBones; }
	vector<Bone*>& GetBonesModified() { return mBones; }

	shared_ptr<Skeleton> Clone();

public:
	static shared_ptr<Skeleton> LoadFrom( Stream& source );

private:
	vector<Bone*> mBones;
};

} // Namespace RcEngine

#endif // Skeleton_h__