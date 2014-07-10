#ifndef Animation_h__
#define Animation_h__

#include <Core/Prerequisites.h>
#include <GamePlay/Component.h>

namespace RcEngine {

class Bone;
class Skeleton;

class _ApiExport AnimationPlayer : public Component
{
	friend class AnimationState;

public:
	AnimationPlayer();
	~AnimationPlayer();

	AnimationState* AddClip( const shared_ptr<AnimationClip>& clip );
	
	AnimationState* GetClip( const String& clip ) const;
	const unordered_map<String, AnimationState*>& GetAllClip() const { return mAnimationStates; }

	void PlayClip(const String& clipName);
	void PauseClip(const String& clipName);
	void ResumeClip(const String& clipName);
	void StopClip(const String& clipName);
	void StopAll();
	
	bool IsPlaying(const String& clipName) const;

public:
	EventHandler EventAnimationCompleted;

protected:

	AnimationState* mCurrentClipState;
	unordered_map<String, AnimationState*> mAnimationStates;
	unordered_map<String, Node*> mAnimateTargets;
};

class _ApiExport SkinnedAnimationPlayer : public AnimationPlayer
{
public:
	SkinnedAnimationPlayer(const shared_ptr<Skeleton>& skeleton);
	~SkinnedAnimationPlayer();

	const String& GetComponentName() const  { return ComponentName; } 

	void CrossFade( const String& fadeClip, float fadeLength );

public:
	static const String ComponentName;	
};

}





#endif // Animation_h__
