#ifndef Player_h__
#define Player_h__

#include <Core/Prerequisites.h>
#include <Graphics/Animation.h>

using namespace RcEngine;

class Player
{
public:
	enum State
	{
		Standby = 0,
		Walk,
		Run,
		Random,
		Casting,
		Fighting_Standby,
		State_Count
	};


public:
	Player();
	~Player();

	void LoadContent();
	void Update(float dt);

private:
	SkinnedAnimationPlayer* mAnimationPlayer;
	Entity* mActorEntity;
	SceneNode* mActorNode;

	AnimationState* mAnimationState[State_Count];
};

#endif // Player_h__
