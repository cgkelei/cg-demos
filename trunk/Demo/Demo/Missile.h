#pragma once
#ifndef _Missile__H
#define _Missile__H

#include "Weapon.h"
#include "Common.h"
#include "BulletCommon.h"

class Missile : public Weapon
{
public:
	Missile(SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level, bool isLeftMissile);
	~Missile(void);

	virtual GameObjectType GetObjectType() const;
	virtual void Update(Real timeSinceLastFrame);
	virtual void Fire( const Ogre::Vector3& startPostion, Ogre::Vector3& direction );



	void Hit() {mIsHit = true; mWeaponSceneNode->setVisible(false);}
	bool IsHit() const { return mIsHit; }

private:
	Real		mBulletScale;
	Entity*		mMissileEntity;
	ParticleSystem* mThrusters;
	SceneNode* mThrustersNode;
	
	bool		mIsHit;
	bool		mIsEplosion;

	// Bullet
	RigidBody*	mRigidbody;
	CollisionShape*	mShape;
	float mRestitution;
	float mFriction;
	float mMass;

	Quaternion mOrientation;

	Vector3 mInitialMoment;
	Matrix3 m_fireOrientation;

};

#endif // _Missile__H