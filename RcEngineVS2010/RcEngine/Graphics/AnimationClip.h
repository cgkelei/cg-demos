#ifndef AnimationClip_h__
#define AnimationClip_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Resource/Resource.h>

namespace RcEngine {

class _ApiExport AnimationClip : public Resource
{
public:
	friend class AnimationPlayer;
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
	AnimationClip(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group );
	~AnimationClip();

	const String& GetClipName() const { return mClipName; }

	/**
	 * Get AnimationClip's duration in seconds
	 */
	float GetLength() const { return mLength; }


protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

private:	
	float mLength;
	String mClipName;
	vector<AnimationTrack> mAnimationTracks;
};



}


#endif // AnimationClip_h__