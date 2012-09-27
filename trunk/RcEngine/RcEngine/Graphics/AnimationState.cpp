#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/Animation.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>

namespace RcEngine {

AnimationState::AnimationState( Animation& animation, AnimationClip* clip )
	: mAnimation(animation), mClip(clip)
{

}

AnimationState::~AnimationState()
{

}

void AnimationState::SetAnimationWrapMode( AnimationWrapMode wrapMode )
{

}

void AnimationState::SetEnable( bool enabled )
{

}

const String& AnimationState::GetName() const
{
	return mClip->GetName();
}

float AnimationState::GetLength() const
{
	return mClip->GetLength();
}

void AnimationState::AdvanceTime( float delta )
{
	float length = mClip->GetLength();

	if (delta == 0 || length == 0)
		return;

	float timePos = GetTime() + delta;

	if (mWrapMode == Wrap_Loop)
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

		if (mEnable)
			mAnimation.mParentMesh.MarkAnimationDirty();
	}
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
			if (mWrapMode != Wrap_Loop)
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
			 bone->SetPosition( Lerp(bone->GetPosition(), keyframe.Translation, mWeight) );
			 bone->SetRotation( QuaternionSlerp(bone->GetRotation(), keyframe.Rotation, mWeight) );
			 bone->SetScale( Lerp(bone->GetScale(), keyframe.Scale, mWeight) );
		}
		else
		{
			const AnimationClip::KeyFrame& nextKeyframe = animTrack.KeyFrames[nextFrame];

			float timeInterval = nextKeyframe.Time - keyframe.Time;
			if (timeInterval < 0.0f)
				timeInterval += mClip->GetLength();
			float t = timeInterval > 0.0f ? (mTime - keyframe.Time) / timeInterval : 1.0f;
		
			bone->SetPosition( Lerp(bone->GetPosition(),
				Lerp(keyframe.Translation, nextKeyframe.Translation, t), mWeight) );

			bone->SetRotation( QuaternionSlerp(bone->GetRotation(), 
				QuaternionSlerp(keyframe.Rotation, nextKeyframe.Rotation, t), mWeight) );

			bone->SetScale( Lerp(bone->GetScale(),
				Lerp(keyframe.Scale, nextKeyframe.Scale, t), mWeight) );
		}
	}
}



}