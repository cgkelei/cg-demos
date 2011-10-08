#pragma once

#include "Common.h"


enum GameObjectType
{
	GOT_Player,
	GOT_Enemy,
	GOT_Bullet,
	GOT_Missile,
	GOT_Other
};


class ShipGameObject
{
public:
	ShipGameObject(void);
	virtual ~ShipGameObject(void);

	virtual GameObjectType GetObjectType() const  = 0;
	virtual void Update(Real timeSinceLastFrame) = 0;
};

