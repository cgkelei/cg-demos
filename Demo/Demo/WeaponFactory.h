#pragma once

#include "Common.h"
#include "Weapon.h"

typedef std::pair<WeaponType, WeaponLevel> WeaponDefinition;
typedef std::list<Weapon*> WeaponList;
typedef std::map<WeaponDefinition, std::list<Weapon*> > WeaponMap;

class WeaponManager : public Ogre::Singleton<WeaponManager>
{
public:
	WeaponManager(void);
	~WeaponManager(void);

	Missile* CreateMissile(ShipGameObject* owner, WeaponLevel level );
	Bullet* CreateBullet(ShipGameObject* owner, WeaponLevel level );
	void Clean();

	static WeaponManager* getSingletonPtr(void);
	static WeaponManager& getSingleton(void);

private:

	WeaponMap mWeaponMap;

};

