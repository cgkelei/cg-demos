#ifndef AnimationState_h__
#define AnimationState_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

enum AnimationWrapMode
{
	AWM_Once,
	AWM_Loop,
};

class AnimationClip;

class _ApiExport AnimationState
{
public:
	AnimationState();

	void SetAnimationWrapMode( AnimationWrapMode wrapMode );
	AnimationWrapMode GetAnimationWrapMode() const  { return mWrapMode; }

	void SetEnable( bool enabled );
	bool IsEnable() const { return mEnable} 

private:
	float mWeight;
	bool mEnable;

	//The length of the animation clip in seconds.
	float mLength; 

	// The current time of the animation.
	float mTime;

	// The name of the animation
	String mName;

	// The clip that is being played by this animation state.
	shared_ptr<AnimationClip> mClip;

	// The playback speed of the animation. 1 is normal playback speed.
	float mSpeed;

	AnimationWrapMode mWrapMode;

};

}


#endif // AnimationState_h__