#ifndef AnimationState_h__
#define AnimationState_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class AnimationClip;
class Animation;

class _ApiExport AnimationState
{
public:
	enum AnimationWrapMode
	{
		Wrap_Once,
		Wrap_Loop,
	};

public:
	AnimationState(Animation& animation, AnimationClip* clip);
	~AnimationState();

	void SetAnimationWrapMode( AnimationWrapMode wrapMode );
	AnimationWrapMode GetAnimationWrapMode() const  { return mWrapMode; }

	void SetEnable( bool enabled );
	bool IsEnabled() const { return mEnable; } 

	/**
	 * Get the state clip name.
	 */
	const String& GetName() const; 

	/**
	 * Set time position.
	 */
	void SetTime( float time );
	
	/**
	 * Get current animation time position.
	 */
	float GetTime() const { return mTime; }

	/**
	 * Advance the animation by delta time.
	 */
	void AdvanceTime( float delta );

	/**
	 * Get animation length in seconds.
	 */
	float GetLength() const;

	float GetWeight() const { return mWeight; }

	uint8_t GetLayer() const { return mLayer; }

	/**
	 * Applies an animation track to the designated target. 
	 */
	void Apply();
	
private:

	Animation& mAnimation;

	// The weight of animation
	float mWeight;

	bool mEnable;

	// The current time of the animation.
	float mTime;

	// The clip that is being played by this animation state.
	AnimationClip* mClip;

	// The playback speed of the animation. 1 is normal playback speed.
	float mSpeed;

	// Wrapping mode of the animation.
	AnimationWrapMode mWrapMode;

	// The layer of the animation. When calculating the final blend weights, animations in higher layers will get their weights
	uint8_t mLayer;
};

}


#endif // AnimationState_h__