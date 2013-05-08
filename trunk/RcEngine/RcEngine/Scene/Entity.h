#ifndef Entity_h__
#define Entity_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Renderable.h>
#include <Graphics/Skeleton.h>

namespace RcEngine {

class SubEntity;
class AnimationPlayer;
class SkinnedAnimationPlayer;

/** 
 * Defines an instance of a discrete, scene object based on a Mesh
 */
class _ApiExport Entity : public SceneObject 
{
	friend class SubEntity;

public:
	Entity( const String& name, const shared_ptr<Mesh>& mesh );
	~Entity();
	
	const BoundingBoxf& GetWorldBoundingBox() const;
	const BoundingBoxf& GetLocalBoundingBox() const;

	const shared_ptr<Mesh>& GetMesh() const							{ return mMesh; }

	uint32_t GetNumSubEntities() const								{ return mSubEntityList.size(); }

	SubEntity* GetSubEntity( uint32_t index ) const					{ return mSubEntityList[index]; }

	void SetDisplaySkeleton(bool display)							{ mDisplaySkeleton = display; }

	bool GetDisplaySkeleton(void) const								{ return mDisplaySkeleton; }

	bool HasSkeleton() const;
	shared_ptr<Skeleton> GetSkeleton();

	bool HasSkeletonAnimation() const;
	AnimationPlayer* GetAnimationPlayer() const;

	void OnAttach( Node* node );
	void OnDetach( Node* node );

	void OnUpdateRenderQueue(RenderQueue* renderQueue, Camera* cam, RenderOrder order);

	Bone* AttachObjectToBone (const String &boneName, SceneObject* sceneObj, const Quaternionf& offsetOrientation= Quaternionf::Identity(), const Vector3f & offsetPosition = Vector3f::Zero());

protected:
	void Initialize();
	void UpdateAnimation();


public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params = 0);

protected:
	shared_ptr<Mesh> mMesh;
	shared_ptr<Skeleton> mSkeleton;
	
	vector<SubEntity*> mSubEntityList;

	unordered_map<String, SceneObject*> mChildAttachedObjects;
	
	bool mDisplaySkeleton;

	mutable BoundingBoxf mWorldBoundingBox;

	vector<Matrix4f> mBoneWorldMatrices;
	vector<Matrix4f> mSkinMatrices;
	uint32_t mNumSkinMatrices;

	SkinnedAnimationPlayer* mAnimationPlayer;
};


}


#endif // Entity_h__
