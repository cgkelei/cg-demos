#include "Weapon.h"
#include "ShipGame.h"
#include "GameLevel.h"

Weapon::Weapon( SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level )
	: mSceneMgr(sceneMgr), mWeaponOwner(owner), mWeaponType(type), mWeaponLevel(level)
{
	mWeaponSceneNode = GameLevel::getSingleton().GetMainNode()->createChildSceneNode();

}


Weapon::~Weapon(void)
{
	
}


void Weapon::Fire( const Ogre::Vector3& startPostion, Ogre::Vector3& direction )
{
	mPosition = startPostion;
	mFireDirection = direction;
}

ShipGameObject* Weapon::GetOwner()
{
	return mWeaponOwner;
}

