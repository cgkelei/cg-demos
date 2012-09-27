#include <Graphics/Animation.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Skeleton.h>
#include <Graphics/Mesh.h>
#include <IO/FileStream.h>

namespace RcEngine {

Animation::Animation( Mesh& mesh )
	: mParentMesh(mesh)
{
	shared_ptr<Skeleton> skeleton = mesh.GetSkeleton();

	vector<Bone*> bones = skeleton->GetBones();
	for (auto iter = bones.begin(); iter != bones.end(); ++iter)
	{
		Bone* bone = *iter;
		mAnimateTargets.insert( std::make_pair(bone->GetName(), bone));
	}
}

Animation::~Animation()
{
	for (size_t i = 0; i < mAnimationClips.size(); ++i)
	{
		Safe_Delete(mAnimationClips[i]);
	}
	mDefaultClip = nullptr;
}

AnimationClip* Animation::GetClip( const String& clipName ) const
{
	auto found = std::find_if(mAnimationClips.begin(), mAnimationClips.end(), [&clipName] (AnimationClip* clip)
					{
						return clipName == clip->mName;
					});
	if (found == mAnimationClips.end())
		return nullptr;

	return *found;
}

AnimationState* Animation::AddClip( AnimationClip* clip )
{
	AnimationState* state = new AnimationState(*this, clip);
	
	mAnimationClips.push_back(clip);

	return state;
}

shared_ptr<Animation> RcEngine::Animation::LoadFrom( Mesh& parentMesh, Stream& source )
{
	shared_ptr<Animation> animation = std::make_shared<Animation>(parentMesh);

	// read clips in this animation
	uint32_t numClips = source.ReadUInt();

	for (uint32_t clip = 0; clip < numClips; ++clip)
	{
		AnimationClip* animClip = new AnimationClip;

		animClip->mName = source.ReadString(); 
		animClip->mLength = source.ReadFloat();

		// read track count
		uint32_t numTracks = source.ReadUInt();
		animClip->mAnimationTracks.resize(numTracks);

		for (uint32_t track = 0; track < numTracks; ++track)
		{
			String trackName = source.ReadString(); 

			// this will add a new track if it doesn't exit
			AnimationClip::AnimationTrack& animTrack = animClip->mAnimationTracks[track];

			// read key frame count
			animTrack.Name = trackName;
			size_t numKeyframes = source.ReadUInt();
			animTrack.KeyFrames.resize(numKeyframes);

			for (uint32_t key = 0; key < numKeyframes; ++key)
			{
				AnimationClip::KeyFrame& animKey = animTrack.KeyFrames[key];

				animKey.Time = source.ReadFloat();

				source.Read(&animKey.Translation, sizeof(Vector3f));
				source.Read(&animKey.Rotation, sizeof(Quaternionf));
				source.Read(&animKey.Scale, sizeof(Vector3f));
			}
		}

		animation->AddClip(animClip);
	}

	// set first clip as default clip
	animation->mDefaultClip = animation->mAnimationClips[0];

	return animation;
}


}


