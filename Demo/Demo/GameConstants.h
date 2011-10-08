#ifndef GameConstants_h__
#define GameConstants_h__

#include <OgreString.h>
#include <OgreMath.h>

namespace GameConstants
{
	const Ogre::Real   PlayerShipSpeed = 450;
	const Ogre::String PlayerMeshName = Ogre::String("razor.mesh");
	
	const Ogre::String F16MissileMeshName =Ogre::String("f16_missile.mesh");
	const Ogre::String MissileMeshName =Ogre::String("missile.mesh");

	const Ogre::Real   MissleAcceleration = 2000;
	const Ogre::Real   MaxMissleVelocity = 4000;
	const Ogre::Real   MissileDamageLevelOne = 100;
	const Ogre::Real   MissileCoolDownTime = 3;

	const Ogre::Real   BulletSpeed = 1000;
	const Ogre::Real   BulletDamageLevelOne = 100;
	const Ogre::Real   BulletCoolDownTime = 3;
	const Ogre::ColourValue BulletColorLevelOne(1, 0, 0, 1);
	const Ogre::Real   BulletLevelOneRange = 2;

	const Ogre::Degree PlayerWheelSpeed = Ogre::Degree(90);
	const Ogre::Degree PlayerClimbSpeed = Ogre::Degree(90);
}


#endif // GameConstants_h__
