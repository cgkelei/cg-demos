#include "Bullet.h"
#include "ShipGame.h"
#include "GameConstants.h"


Bullet::Bullet( SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level )
	: Weapon(sceneMgr, owner, type, level)
{
	mBillboardSet = NULL;
	mBillboard = NULL;
	mBulletScale = 10;
	mIsFire = false;	
}


Bullet::~Bullet(void)
{
	mSceneMgr->getRootSceneNode()->detachObject(mRibbonTrail);
	mBillboardSet = NULL;
	mBillboard = NULL;
}


void Bullet::Fire( const Ogre::Vector3& startPostion, Ogre::Vector3& direction )
{
	Weapon::Fire(startPostion, direction);

	Ogre::SceneManager* mSceneMgr = ShipGame::getSingleton().GetSceneManager();
	
	// create a ribbon trail that our lights will leave behind
	Ogre::NameValuePairList params;
	params["numberOfChains"] = "1";
	params["maxElements"] = "80";

	mRibbonTrail = (Ogre::RibbonTrail*)mSceneMgr->createMovableObject("RibbonTrail", &params);
	mSceneMgr->getRootSceneNode()->attachObject(mRibbonTrail);
	mRibbonTrail->setMaterialName("Examples/LightRibbonTrail");
	mRibbonTrail->setTrailLength(200);

	// set initial settings for the ribbon trail and add the light node
	mRibbonTrail->setInitialColour(0, mColour);
	mRibbonTrail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
	mRibbonTrail->setInitialWidth(0, 5);

	mRibbonTrail->addNode(mWeaponSceneNode);

	mFireRange = GameConstants::BulletLevelOneRange;
	
	mIsFire = true;
	
}


GameObjectType Bullet::GetObjectType() const
{
	return GOT_Bullet;
}

void Bullet::Update( Real timeSinceLastFrame )
{
	if(mIsFire)
	{
		mPosition += GameConstants::BulletSpeed * mFireDirection * timeSinceLastFrame;
		mWeaponSceneNode->setPosition(mPosition);

		mFireRange -= timeSinceLastFrame;
	}
}

