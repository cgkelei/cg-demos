#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>
#include <Input/InputSystem.h>
#include <Core/Context.h>

namespace RcEngine {

AnimationState::AnimationState( AnimationPlayer& animation, const shared_ptr<AnimationClip> clip )
	: mAnimation(animation), mClip(clip), mTime(0), BlendWeight(1.0f), PlayBackSpeed(1.0f), WrapMode(Wrap_Once),
	mStateBits(0x00)
{

}

AnimationState::~AnimationState()
{

}

void AnimationState::SetAnimationWrapMode( AnimationWrapMode wrapMode )
{
	WrapMode = wrapMode;
}

void AnimationState::SetEnable( bool enabled )
{
	mEnable = enabled;
}

const String& AnimationState::GetName() const
{
	return mClip->GetName();
}

float AnimationState::GetLength() const
{
	return mClip->GetLength();
}

void AnimationState::Apply()
{
	if (!IsEnabled())
		return;

	for (auto iter = mClip->mAnimationTracks.begin(); iter != mClip->mAnimationTracks.end(); ++iter)
	{
		const AnimationClip::AnimationTrack& animTrack = *iter;
		
		// if no key frames, pass
		if (animTrack.KeyFrames.empty())
			continue;

		auto found = mAnimation.mAnimateTargets.find(animTrack.Name);

		assert( found != mAnimation.mAnimateTargets.end() );

		Bone* bone = found->second;

		size_t frame = animTrack.GetKeyFrameIndex(mTime);

		size_t nextFrame = frame + 1;

		bool interpolate = true;
		if (nextFrame >= animTrack.KeyFrames.size())
		{
			if (WrapMode != Wrap_Loop)
			{
				nextFrame = frame;
				interpolate = false;
			}
			else
				nextFrame = 0;
		}

		const AnimationClip::KeyFrame& keyframe = animTrack.KeyFrames[frame];

		if (!interpolate)
		{
			// No interpolation, blend between old transform & animation
			 bone->SetPosition( Lerp(bone->GetPosition(), keyframe.Translation, BlendWeight) );
			 bone->SetRotation( QuaternionSlerp(bone->GetRotation(), keyframe.Rotation, BlendWeight) );
			 bone->SetScale( Lerp(bone->GetScale(), keyframe.Scale, BlendWeight) );
		}
		else
		{
			const AnimationClip::KeyFrame& nextKeyframe = animTrack.KeyFrames[nextFrame];

			//std::cout << keyframe.Time << "-->" << nextKeyframe.Time << std::endl;

			float timeInterval = nextKeyframe.Time - keyframe.Time;
			if (timeInterval < 0.0f)
				timeInterval += mClip->GetLength();
			float t = timeInterval > 0.0f ? (mTime - keyframe.Time) / timeInterval : 1.0f;
		
			bone->SetPosition( Lerp(bone->GetPosition(),
			Lerp(keyframe.Translation, nextKeyframe.Translation, t), BlendWeight) );

			bone->SetRotation( QuaternionSlerp(bone->GetRotation(), 
			QuaternionSlerp(keyframe.Rotation, nextKeyframe.Rotation, t), BlendWeight) );

			bone->SetScale( Lerp(bone->GetScale(),
			Lerp(keyframe.Scale, nextKeyframe.Scale, t), BlendWeight) );
		}
	}
}

void AnimationState::AdvanceTime( float delta )
{
	float length = mClip->GetLength();

	if (delta == 0 || length == 0)
		return;

	float timePos = GetTime() + delta;

	if (WrapMode == Wrap_Loop)
	{
		// wrap
		timePos = fmod(timePos, length);
		if(timePos < 0)
			timePos += length;    
	}

	SetTime(timePos);
}

void AnimationState::SetTime( float time )
{
	float length = mClip->GetLength();

	// Clamp
	if(time < 0)
		time = 0;
	else if (time > GetLength())
		time = length;

	if (mTime != time)
	{
		mTime = time;

		//if (mEnable)
		//	mAnimation.mParentMesh.MarkAnimationDirty();
	}
}

void AnimationState::SetFadeLength( float fadeLength )
{
	FadeLength = fadeLength;
}

bool AnimationState::Update( float delta )
{
	if (IsClipStateBitSet(Clip_Is_Pause_Bit))
	{
		// paused
		return false;
	}
	else if (IsClipStateBitSet(Clip_Is_End_Bit))
	{
		// End
		OnEnd();
		return true;
	}
	else if (!IsClipStateBitSet(Clip_Is_Started_Bit))
	{
		// first time update, begin
		OnBegin();
		SetClipStateBit(Clip_Is_Started_Bit);
	}
	else
	{
		// advance time
		AdvanceTime(delta * PlayBackSpeed);
	}

	float percentComplete = mTime / GetLength();

	if (IsClipStateBitSet(Clip_Is_Fading_Bit))
	{
		assert(FadeLength > 0);

		mCrossFadeOutElapsed += delta * PlayBackSpeed;

		if (mCrossFadeOutElapsed < FadeLength)
        {
            // Calculate this clip's blend weight.
            float tempBlendWeight = (FadeLength - mCrossFadeOutElapsed) / FadeLength;
            
            // If this clip is fading in, adjust the crossfade clip's weight to be a percentage of your current blend weight
            if (IsClipStateBitSet(Clip_Is_FadeIn_Started_Bit))
            {
                BlendWeight = tempBlendWeight;
            }
            else
            {
                // Just set the blend weight.
                BlendWeight = 1 - tempBlendWeight;
            }
        }
        else
        {  
			if (IsClipStateBitSet(Clip_Is_FadeIn_Started_Bit))
			{
				BlendWeight = 1.0f;
				ResetClipStateBit(Clip_Is_FadeIn_Started_Bit);
				ResetClipStateBit(Clip_Is_Fading_Bit);
			}
			else
			{
				BlendWeight = 0.0f;
				ResetClipStateBit(Clip_Is_FadeOut_Started_Bit);
				ResetClipStateBit(Clip_Is_Fading_Bit);
			}
			
        }
	}

	return true;
}


bool AnimationState::IsClipStateBitSet( uint8_t bits ) const
{
	return (mStateBits & bits) == bits ;
}

void AnimationState::SetClipStateBit( uint8_t bits )
{
	mStateBits |= bits;
}

void AnimationState::ResetClipStateBit( uint8_t bits )
{
	mStateBits &= ~bits;
}

void AnimationState::OnBegin()
{

}

void AnimationState::OnEnd()
{

}



}