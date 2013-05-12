#include <Graphics/Animation.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>
#include <Scene/SceneManager.h>
#include <IO/FileStream.h>
#include <Core/Context.h>
#include <Core/Exception.h>

namespace RcEngine {

AnimationPlayer::AnimationPlayer( )
{
	mController = Context::GetSingleton().GetSceneManager().GetAnimationController();
}

AnimationPlayer::~AnimationPlayer()
{
	for (auto iter = mAnimationStates.begin(); iter != mAnimationStates.end(); ++iter)
	{
		delete iter->second;
	}
	mAnimationStates.clear();
}

AnimationState* AnimationPlayer::GetClip( const String& clipName ) const
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
		return nullptr;

	return found->second;
}


AnimationState* AnimationPlayer::AddClip( const shared_ptr<AnimationClip>& clip )
{
	auto found = mAnimationStates.find(clip->GetClipName());

	if (found != mAnimationStates.end())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Same animation clip exits", "Animation::AddClip");
	}

	if (!clip->IsLoaded())
		clip->Load();

	AnimationState* newClipState = new AnimationState(*this, clip);
	mAnimationStates.insert( std::make_pair(clip->GetClipName(),  newClipState) );

	return newClipState;
}

void AnimationPlayer::StopAll()
{
	for (auto iter = mAnimationStates.begin(); iter != mAnimationStates.end(); ++iter)
	{
		AnimationState* clipState = iter->second;

		if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
		{
			// Mark the clip to removed from the AnimationController.
			clipState->SetClipStateBit(AnimationState::Clip_Is_End_Bit);
		}
	}
}

void AnimationPlayer::PlayClip( const String& clipName )
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
	{
		return;
	}

	AnimationState* clipState = found->second;

	if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
	{
		if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Pause_Bit))
		{
			clipState->ResetClipStateBit(AnimationState::Clip_Is_Pause_Bit);
		}
	}
	else
	{
		clipState->SetClipStateBit(AnimationState::Clip_Is_Playing_Bit);

		// add to controller
		mController->Schedule(clipState);
	}
}

void AnimationPlayer::PauseClip( const String& clipName )
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
	{
		return;
	}

	AnimationState* clipState = found->second;

	if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit) &&
		!clipState->IsClipStateBitSet(AnimationState::Clip_Is_End_Bit))
	{
		clipState->SetClipStateBit(AnimationState::Clip_Is_Pause_Bit);
	}
}

void AnimationPlayer::ResumeClip( const String& clipName )
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
	{
		return;
	}

	AnimationState* clipState = found->second;

	if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Pause_Bit))
	{
		clipState->SetClipStateBit(AnimationState::Clip_Is_Playing_Bit);
	}
}

bool AnimationPlayer::IsPlaying( const String& clipName ) const
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
	{
		return false;
	}

	AnimationState* clipState = found->second;

	return clipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit);
}

void AnimationPlayer::StopClip( const String& clipName )
{
	auto found = mAnimationStates.find(clipName);

	if (found == mAnimationStates.end())
	{
		return;
	}

	AnimationState* clipState = found->second;

	if (clipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
	{
		// Mark the clip to removed from the AnimationController.
		clipState->SetClipStateBit(AnimationState::Clip_Is_End_Bit);
	}
}

SkinnedAnimationPlayer::SkinnedAnimationPlayer( const shared_ptr<Skeleton>& skeleton )
{
	assert(skeleton != nullptr);

	vector<Bone*> bones = skeleton->GetBones();
	for (auto iter = bones.begin(); iter != bones.end(); ++iter)
	{
		Bone* bone = *iter;
		mAnimateTargets.insert( std::make_pair(bone->GetName(), bone));
	}
}


void SkinnedAnimationPlayer::CrossFade( const String& fadeClip, float fadeLength )
{
	//AnimationState* fadeClipState;

	//auto found = mAnimationStates.find(fadeClip);
	//assert(found != mAnimationStates.end());
	//fadeClipState = found->second;


	//if (!fadeClipState->IsClipStateBitSet(AnimationState::Clip_Is_Fading_Bit) &&
	//	!mCurrentClipState->IsClipStateBitSet(AnimationState::Clip_Is_Fading_Bit))
	//{
	//	// if the given clip is not fading, do fading	
	//	fadeClipState->BlendWeight = 0.0f;
	//	fadeClipState->SetClipStateBit( AnimationState::Clip_Is_FadeIn_Started_Bit | AnimationState::Clip_Is_Fading_Bit );

	//	mCurrentClipState->FadeLength = fadeLength;
	//	mCurrentClipState->ResetCrossFadeTime();

	//	mCurrentClipState->SetClipStateBit( AnimationState::Clip_Is_FadeOut_Started_Bit | AnimationState::Clip_Is_Fading_Bit );

	//	// If this clip is currently not playing, we should start playing it.
	//	if (!mCurrentClipState->IsClipStateBitSet(AnimationState::Clip_Is_Playing_Bit))
	//	{
	//		PlayClip(mCurrentClipState->GetName());
	//	}

	//	// Start playing the cross fade clip.
	//	PlayClip(fadeClip);

	//}
}

SkinnedAnimationPlayer::~SkinnedAnimationPlayer()
{

}


}


