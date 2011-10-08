#pragma once
#ifndef _GameObject__H
#define _GameObject__H

#include "Common.h"
#include "ShipGameObject.h"
#include "BulletCommon.h"

class Enemy : public ShipGameObject
{
public:
	Enemy(SceneManager* sceneMrg, const String& meshName, const Vector3& startPos,
	 const Vector3& scale, const Quaternion& startQuaternion, const String& material);
	~Enemy(void);

	virtual GameObjectType GetObjectType() const;
	virtual void Update(Real timeSinceLastFrame);
	bool IsDead() const { return mIsDead; }

protected:

	SceneManager* mSceneMgr;
	SceneNode* mEnemyNode;
	Entity*	mEnemyEntity;

	RigidBody* mRigidbody;
	CollisionShape* mShape;

	Vector3 mPosition;
	String mName;
	String mMeshName;

	bool mIsDead;

	// Bullet params
	float mRestitution;
	float mFriction;
	float mMass;
	Quaternion mOrientation;

};

#endif