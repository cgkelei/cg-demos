#ifndef AnimationClip_h__
#define AnimationClip_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>

namespace RcEngine {

class AnimationState;
class Bone;

class _ApiExport AnimationClip
{
	friend class Animation;
	friend class AnimationState;

	struct _ApiExport KeyFrame
	{
		// The time offset from the start of the animation to this keyframe.
		float Time;

		Vector3f Translation;
		Quaternionf Rotation;
		Vector3f Scale;
	};

	struct _ApiExport AnimationTrack
	{
		// Bone name
		String Name;
		vector<KeyFrame> KeyFrames;

		size_t GetKeyFrameIndex( float time ) const;
	};

public:
	AnimationClip();
	~AnimationClip();

	const String& GetName() const { return mName; }

	/**
	 * Get AnimationClip's duration in seconds
	 */
	float GetLength() const { return mLength; }
	
	/**
	 * Get frame rate at which keyframes are sampled.
	 */
	float GetFrameRate() const;

	AnimationState* GetAnimationState() const { return mAnimationState; }
	void SetAnimationState( AnimationState* state );

private:
	String mName;
	float mLength;
	vector<AnimationTrack> mAnimationTracks;

	AnimationState* mAnimationState;
};



}


#endif // AnimationClip_h__
