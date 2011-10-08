#pragma once
#ifndef _Player__H
#define _Player__H

#include "Common.h"
#include "BulletCommon.h"
#include "ShipGameObject.h"
#include "Weapon.h"

class Player : public ShipGameObject
{
public:
	Player(SceneManager* sceneMgr);
	~Player(void);

	virtual GameObjectType GetObjectType() const;
	virtual void Update(Real timeSinceLastFrame);

	Ogre::Degree GetWheelDegree()				{ return mWheelDegree; }
	Ogre::SceneNode* GetPlayerSceneNode()		{ return mPlayerSceneNode; }
	Ogre::Vector3 GetDirection() const;

private:

	void ProcessWeaponFireProcess(Real timeSinceLastFrame);

private:
	String mName;

	SceneManager* mSceneMgr;
	Ogre::Entity*		mSpaceShipEntity;
	Ogre::SceneNode*    mPlayerSceneNode;
	
	// Bullet params
	RigidBody* mRigidbody;
	CollisionShape* mShape;
	float mRestitution;
	float mFriction;
	float mMass;

	Ogre::Degree        mWheelDegree;
	Ogre::Degree		mPitch;

	float				mThrottle;

	Ogre::Quaternion	mOrientation;
	Ogre::Vector3		mPosition;
	Ogre::Vector3		mDirection;
	Ogre::Matrix3		mWorldTransform;

	
	WeaponLevel		   mMissileLevel;
	Ogre::Real		   mMissileCoolDownTime;

	WeaponLevel		   mBulletLevel;
	Ogre::Real		   mBulletCoolDownTime;

	// 发左边还是右边的导弹
	bool				mWhichMissile;
};


#endif // _Player__H