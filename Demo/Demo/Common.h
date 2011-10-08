#ifndef _Common__H
#define _Common__H

#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <Ogre.h>
#include <OgreCommon.h>
#include <OgreSingleton.h>
#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>

using namespace Ogre;

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }

#define OGRE_SINGLETON_DECLARE(Name) static Name* getSingletonPtr(void); \
									 static Name& getSingleton(void);    
									 

#define OGRE_SINGLETON_IMPLEMNET(Name) template<> Name * Ogre::Singleton< Name >::ms_Singleton = 0; \
										Name* Name::getSingletonPtr(void)							\
										{ return ms_Singleton; }									\
									    Name& Name::getSingleton(void)								\
										{  assert( ms_Singleton );  return ( *ms_Singleton );  }	


class ShipGame;
class GameLevel;
class GameTerrain;
class PhysicsEngine;
class Weapon;
class Bullet;
class Missile;
class Player;
class Enemy;
class GameState;


#endif // _Common__H
