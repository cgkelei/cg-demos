#ifndef AnimationController_h__
#define AnimationController_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class AnimationClip;

class _ApiExport AnimationController
{
public:
	enum State
	{
		Running,
		Idle,
		Paused,
		Stopped
	};

public:
	AnimationController();
	~AnimationController();

	
	State GetState() const	{ return mState; }

	void Update(float elapsedTime);

	void Pause();
	void Resume();

private:
	State mState;

	// A list of running AnimationClips.
	std::list<AnimationClip*> mRunningClips;     
};

}


#endif // AnimationController_h__
