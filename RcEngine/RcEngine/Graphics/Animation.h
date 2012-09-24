#ifndef Animation_h__
#define Animation_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>

namespace RcEngine {


struct _ApiExport KeyFrame
{
	//KeyFrame() { }

	// The time offset from the start of the animation to this keyframe.
	float Time;

	Vector3f Translation;
	Quaternionf Rotation;
	Vector3f Scale;
};

struct _ApiExport AnimationTrack
{
	AnimationTrack() {}
	~AnimationTrack() {} 

	// Bone name
	String Name;
	vector<KeyFrame> KeyFrames;
};

struct _ApiExport AnimationClip
{	
	float Duration;
	vector< AnimationTrack > AnimationTracks;
};

class _ApiExport Animation
{
public:
	Animation();

	void AddClip( const String& clipName, const shared_ptr<AnimationClip> clip );
	void RemoveClip( const String& clipName );

	shared_ptr<Animation> LoadFrom(Stream& source);

private:
	unordered_map<String, shared_ptr<AnimationClip> > mAnimationClips;
};


}





#endif // Animation_h__
