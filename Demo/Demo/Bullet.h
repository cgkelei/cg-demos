#pragma once

#include "Weapon.h"
#include "Common.h"
#include "BulletCommon.h"

class Bullet : public Weapon
{
public:
	Bullet(SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level);
	virtual ~Bullet(void);

	virtual GameObjectType GetObjectType() const;
	virtual void Update(Ogre::Real timeSinceLastFrame);
	virtual void Fire(const Ogre::Vector3& startPostion, Ogre::Vector3& direction);

private:

	Ogre::Real				mBulletScale;

	Ogre::ColourValue		mColour;
	Ogre::BillboardSet*		mBillboardSet;
	Ogre::Billboard*		mBillboard;
	Ogre::RibbonTrail*		mRibbonTrail;
	Ogre::Real				mFireRange;	
	bool					mIsFire;

};

