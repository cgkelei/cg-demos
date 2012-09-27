#include <Graphics/AnimationController.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationClip.h>

namespace RcEngine {

AnimationController::AnimationController()
{

}

AnimationController::~AnimationController()
{

}

void AnimationController::Update( float elapsedTime )
{
	if (mState != Running)
		return;

	// Loop through running clips and call update() on them.
	for (auto iter = mRunningClips.begin(); iter != mRunningClips.end(); ++iter)
	{
		AnimationClip* clip = *iter;

		AnimationState* clipState = clip->GetAnimationState();
		
	}
}

}