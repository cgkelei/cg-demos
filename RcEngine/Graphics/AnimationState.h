#ifndef AnimationState_h__
#define AnimationState_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class AnimationClip;

class _ApiExport AnimationState
{
public:
	enum AnimationWrapMode
	{
		Wrap_Once,
		Wrap_Loop,
	};      

	enum AnimationStateBits
	{
		Clip_Is_Playing_Bit = 0x1,
		Clip_Is_Started_Bit = 0x2,
		Clip_Is_Pause_Bit = 0x4,
		Clip_Is_End_Bit = 0x08,
		Clip_Is_FadeOut_Started_Bit = 0x10,
		Clip_Is_FadeIn_Started_Bit = 0x20,
		Clip_Is_Fading_Bit = 0x30,
		Clip_All_Bit = 0xFF
	};

public:
	AnimationState(AnimationPlayer& animation, const shared_ptr<AnimationClip> clip);
	~AnimationState();

	void SetAnimationWrapMode( AnimationWrapMode wrapMode );
	AnimationWrapMode GetAnimationWrapMode() const  { return WrapMode; }

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

	bool Update(float delta);

	/**
	 * Advance the animation by delta time.
	 */
	void AdvanceTime( float delta );

	/**
	 * Get animation length in seconds.
	 */
	float GetLength() const;

	float GetWeight() const { return BlendWeight; }

	uint8_t GetLayer() const { return mLayer; }
	
	void SetFadeLength(float fadeLength);

	/**
	 * Applies an animation track to the designated target. 
	 */
	void Apply();

	void ResetCrossFadeTime()	{ mCrossFadeOutElapsed = 0.0f; }

	void SetClipStateBit(uint8_t bits);
	void ResetClipStateBit(uint8_t bits);
	bool IsClipStateBitSet(uint8_t bits) const;

private:
	void OnBegin();
	void OnEnd();
	
private:

	AnimationPlayer& mAnimation;

	uint8_t mStateBits;

	bool mEnable;

	// The current time of the animation.
	float mTime;

	// The clip that is being played by this animation state.
	shared_ptr<AnimationClip> mClip;

	float mCrossFadeOutElapsed;

public:

	// The weight of animation
	float BlendWeight;

	// The playback speed of the animation. 1 is normal playback speed.
	float PlayBackSpeed;

	// Wrapping mode of the animation.
	AnimationWrapMode WrapMode;

	// The layer of the animation. When calculating the final blend weights, animations in higher layers will get their weights
	uint8_t mLayer;

	float FadeLength;
	
};

}


#endif // AnimationState_h__