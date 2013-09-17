#ifndef Player_h__
#define Player_h__

#include <Core/Prerequisites.h>
#include <Graphics/Animation.h>

using namespace RcEngine;

class Player
{
public:
	Player();
	~Player();

	void LoadContent();
	void Update(float dt);

private:
	SkinnedAnimationPlayer* mAnimationPlayer;
	Entity* mActorEntity;
	SceneNode* mActorNode;
};

#endif // Player_h__
