#ifndef Animation_h__
#define Animation_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class AnimationClip;
class AnimationController;
class AnimationState;
class Bone;

class _ApiExport Animation
{
	friend class AnimationState;

public:
	Animation(Mesh& mesh);
	~Animation();

	bool IsPlaying( const String& clip );

	void Play( const String& clip = "");
	void CrossFade( const String& clip );
	
	void Stop();
	void Stop( const String& clip );

	void Sample();

	AnimationClip* GetClip( const String& clip ) const;

	AnimationState* AddClip( AnimationClip* clip );


	/**
	 * Set the mesh which use this animation.
	 */
	void SetMesh( const shared_ptr<Mesh>& mesh );

public:
	static shared_ptr<Animation> LoadFrom(Mesh& parentMesh, Stream& source);

private:
	Mesh& mParentMesh;

	AnimationController* mController; 
	AnimationClip* mDefaultClip;
	vector<AnimationClip*> mAnimationClips;	

	// Mapping of animation track indices to bones.
	unordered_map<String, Bone*> mAnimateTargets;
};


}





#endif // Animation_h__
