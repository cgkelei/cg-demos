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
		AnimationState* clipState = *iter;
		if( !clipState->Update(elapsedTime) )
		{
			mRunningClips.erase(iter);
		}
	}

	if (mRunningClips.empty())
		mState = Idle;
}

void AnimationController::Schedule(AnimationState* clipState)
{
	if (mRunningClips.empty())
	{
		mState = Running;
	}

	assert(clipState);
	mRunningClips.push_back(clipState);
}

void AnimationController::Unschedule(AnimationState* clipState)
{
	auto found = std::find(mRunningClips.begin(), mRunningClips.end(), clipState);

	if (found != mRunningClips.end())
	{
		mRunningClips.erase(found);
	}

	if (mRunningClips.empty())
		mState = Idle;
}

}