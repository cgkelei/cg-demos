#include "WeaponFactory.h"
#include "Missile.h"
#include "Bullet.h"
#include "GameConstants.h"
#include "GameState.h"
#include "ShipGame.h"

//---------------------------------------------------------------------
template<> WeaponManager * Ogre::Singleton< WeaponManager >::ms_Singleton = 0;
WeaponManager* WeaponManager::getSingletonPtr(void)
{
	return ms_Singleton;
}
WeaponManager& WeaponManager::getSingleton(void)
{  
	assert( ms_Singleton );  return ( *ms_Singleton );  
}


//---------------------------------------------------------------------

WeaponManager::WeaponManager(void)
{
}


WeaponManager::~WeaponManager(void)
{
	Clean();
}

Missile* WeaponManager::CreateMissile( ShipGameObject* owner, WeaponLevel level )
{
	static bool isLeftMissile = true;
	SceneManager* sceneMgr = ShipGame::getSingleton().GetSceneManager();
	Missile* missile = NULL;
	missile = new Missile(sceneMgr, owner, WT_Missile, level, isLeftMissile);
	isLeftMissile = !isLeftMissile;
	// add to game state
	ShipGame::getSingleton().GetGameState()->AddMissile(missile);
	return missile;
}


Bullet* WeaponManager::CreateBullet( ShipGameObject* owner, WeaponLevel level )
{
	SceneManager* sceneMgr = ShipGame::getSingleton().GetSceneManager();
	Bullet* bullet = NULL;
	bullet = new Bullet(sceneMgr, owner, WT_Bullet, level);

	// add to game state
	ShipGame::getSingleton().GetGameState()->AddBullet(bullet);
	return bullet;
}


void WeaponManager::Clean()
{
	for(WeaponMap::iterator iter = mWeaponMap.begin(); iter != mWeaponMap.end(); ++iter)
	{
		for(WeaponList::iterator weaponIter = iter->second.begin(); 
			weaponIter != iter->second.end(); ++weaponIter)
		{
			delete (*weaponIter);
		}
	}
}
