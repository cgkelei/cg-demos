#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Core/Exception.h>

namespace RcEngine {

AnimationClip::AnimationClip()
	: mAnimationState( nullptr )
{
	
}

AnimationClip::~AnimationClip()
{
	Safe_Delete(mAnimationState);
}

void AnimationClip::SetAnimationState( AnimationState* state )
{
	assert(mAnimationState == nullptr);
	mAnimationState = state;
}

size_t AnimationClip::AnimationTrack::GetKeyFrameIndex( float time ) const
{
	if (time < 0)
		time = 0;
	return 0;

}

}