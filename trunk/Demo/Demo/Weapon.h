#pragma once
#ifndef _Weapon__H
#define _Weapon__H

#include "ShipGameObject.h"

enum WeaponType
{
	WT_Missile,
	WT_Bullet
};

enum WeaponLevel
{
	WL_One,
	WL_Two
};

class Weapon : public ShipGameObject
{
public:
	Weapon(SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level);
	virtual ~Weapon(void);

	virtual void Fire(const Ogre::Vector3& startPostion, Ogre::Vector3& direction);
	ShipGameObject* GetOwner();
	
protected:

	SceneManager* mSceneMgr;
	SceneNode*	  mWeaponSceneNode;

	ShipGameObject* mWeaponOwner;

	WeaponLevel mWeaponLevel;
	WeaponType mWeaponType;

	Vector3	mPosition;
	Vector3	mFireDirection;
	Real mVelocity;

	String mName;
	
};


#endif // _Weapon__H