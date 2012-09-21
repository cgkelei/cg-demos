#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>

namespace RcEngine {

class Skeleton;

class _ApiExport Bone : public Node
{
public:
	Bone( const String& name, Bone* parent = 0 );

protected:
	virtual Node* CreateChildImpl( const String& name ) { return 0; }

private:
	void OnUpdate( );
	

public:
	Vector3f mBindDerivedPosition;
	Vector3f mBindDerivedScale;
	Quaternionf mBindDerivedRotation;
};

class _ApiExport Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void Update( float delta );

	Bone* GetRootBone();
	Bone* GetBone( const String& name );
	Bone* GetBone( uint32_t index );

	vector<Bone*>& GetBones() { return mBones; }

	static shared_ptr<Skeleton> LoadFrom( Stream& source );

private:
	
	vector<Bone*> mBones;

	int32_t mRootBoneIndex;

};

} // Namespace RcEngine

#endif // Skeleton_h__