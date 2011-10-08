#include "PhysicsEngine.h"
#include "ShipGame.h"
#include "ShipGameObject.h"
#include "GameLevel.h"
#include "Enemy.h"
#include "Player.h"
#include "Missile.h"
#include "Bullet.h"
#include "Utilities.h"
#include "ParticleManager.h"

OGRE_SINGLETON_IMPLEMNET(PhysicsEngine)


bool ContactCallback(btManifoldPoint& cp,void* body0, void* body1);

PhysicsEngine::PhysicsEngine( SceneManager* sceneManager, Vector3 &gravityVector, AxisAlignedBox &bounds )
	: mSceneMgr(sceneManager), 
	mPhysicsWorld(NULL),
	mDebugDrawer(NULL), 
	mIsShowDebug(false)
{
	mPhysicsWorld = new DynamicsWorld(sceneManager, bounds, gravityVector);
	gContactProcessedCallback = ContactCallback;
	
	mDebugDrawer = new DebugDrawer();
	mDebugDrawer->setDrawWireframe(true);	// we want to see the Bullet containers
	mPhysicsWorld->setDebugDrawer(mDebugDrawer);
	
	mPhysicsWorld->setShowDebugShapes(mIsShowDebug);		// enable it if you want to see the Bullet containers
	
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (mDebugDrawer));
}


PhysicsEngine::~PhysicsEngine(void)
{
}


void PhysicsEngine::Update( Real timeSinceLastFrame )
{

	static bool flag = false; 
	bool currRightDown = ShipGame::getSingleton().GetKeyboard()->isKeyDown(OIS::KC_Q);
	if ( currRightDown && (! flag) )
	{
		mIsShowDebug = !mIsShowDebug;
		mPhysicsWorld->setShowDebugShapes(mIsShowDebug);	
	}
	flag = currRightDown;

	mPhysicsWorld->stepSimulation(timeSinceLastFrame);
}

void PhysicsEngine::AddBody( RigidBody* body )
{
	BodyListIter iter = std::find(mBodies.begin(), mBodies.end(), body);
	if(iter == mBodies.end())
		mBodies.push_back(body);
}

void PhysicsEngine::AddShape( CollisionShape* shape )
{
	ShapeListIter iter = std::find(mShapes.begin(), mShapes.end(), shape);
	if(iter == mShapes.end())
		mShapes.push_back(shape);
}

DynamicsWorld* PhysicsEngine::GetDynamicsWorld()
{
	return mPhysicsWorld;
}



void PhysicsEngine::PutTerrainData( unsigned int pageSize, Real worldSize, float* data, Real maxHeight, const Ogre::Vector3& position )
{
	
	Ogre::Vector3 terrainScale(worldSize / (pageSize-1), maxHeight, worldSize / (pageSize-1));

	// create terrain shape
	HeightmapCollisionShape* terrainShape =  new HeightmapCollisionShape(pageSize, pageSize, terrainScale, data, true);

	// create body
	RigidBody* defaultTerrainBody = new RigidBody("Terrain", mPhysicsWorld);

	const float      terrainBodyRestitution  = 0.6f;
	const float      terrainBodyFriction     = 0.8f;

	Vector3 terrainShiftPos(0,0,0);	// (terrainScale.x * (page_size - 1) / d2), 0, (terrainScale.z * (page_size - 1) / 2));
	terrainShiftPos.y = terrainScale.y / 2 * terrainScale.y;

	SceneNode* pTerrainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode();
	pTerrainNode->setPosition(position);

	defaultTerrainBody->setStaticShape (pTerrainNode, terrainShape, terrainBodyRestitution, terrainBodyFriction, terrainShiftPos);

	mBodies.push_back(defaultTerrainBody);
	mShapes.push_back(terrainShape);

}

//////////////////////////////////////////////////////////////////////////
///								物理碰撞逻辑代码


void PlayerShipHitEnemy(Player* player, Enemy* emeny)
{

}

void WeaponHitEnemy(Weapon* weapon, Enemy* enemy)
{

}

void EnemyHitEnemy(Enemy* emeny0, Enemy* emeny1) 
{

}

void WeaponHitPlayerShip(Weapon* weapon, Player* player)
{

}

void PlayShipHitTerrain(Player* player)
{

}




bool ContactCallback(btManifoldPoint& cp,void* body0, void* body1)
{
	if (body0 && body1)
	{
		const btRigidBody* colObj0 = static_cast<const btRigidBody*>(body0);
		const btRigidBody* colObj1 = static_cast<const btRigidBody*>(body1);

		// 与地形碰撞代码
		if(colObj0->isStaticObject() || colObj1->isStaticObject())
		{
			ShipGameObject* obj0 = NULL;
			ShipGameObject* obj1 = NULL;

			if (!colObj0->isStaticObject())
			{
				obj0 =  static_cast<ShipGameObject*>( colObj0->getUserPointer() );
				if(obj0->GetObjectType() == GOT_Missile)
				{
					Missile* missle = static_cast<Missile*>(obj0);
					if(!missle->IsHit())
					{
						missle->Hit();
						Vector3 pos = Utils::ToOgreVector3(cp.m_positionWorldOnA);
						ParticleManager::getSingleton().Explosion(pos);
					}
				}
			}
			if (!colObj1->isStaticObject())
			{
				obj1 = static_cast<ShipGameObject*>( colObj1->getUserPointer() );
				if(obj1->GetObjectType() == GOT_Missile)
				{
					Missile* missle = static_cast<Missile*>(obj1);
					if(!missle->IsHit())
					{
						missle->Hit();
						ParticleManager::getSingleton().Explosion(Utils::ToOgreVector3(cp.m_localPointB));
					}
				}
			}
		
	
			
		}

		ShipGameObject* obj0 = static_cast<ShipGameObject*>(colObj0->getUserPointer());
		ShipGameObject* obj1 = static_cast<ShipGameObject*>(colObj1->getUserPointer());

		if(obj0)
		{
			if(obj1)
			{
				const GameObjectType t0 = obj0->GetObjectType();
				const GameObjectType t1 = obj1->GetObjectType();

				if(t0 == GOT_Player)
				{
					Player* player = static_cast<Player*>(obj0);
					if(t1 == GOT_Enemy)
					{
						PlayerShipHitEnemy(player, static_cast<Enemy*>(obj1));
					}
					else if (t1 == GOT_Bullet)
					{
						Bullet* bullet = static_cast<Bullet*>(obj1);
						if(bullet->GetOwner()->GetObjectType() == GOT_Enemy)
						{
							WeaponHitPlayerShip(bullet, player);
						}
					}
				}

				if(t0 == GOT_Enemy)
				{
					Enemy* enemy = static_cast<Enemy*>(obj0);
					switch (t1)
					{
					case GOT_Player:
						PlayerShipHitEnemy(static_cast<Player*>(obj1), enemy);
						break;
					case GOT_Enemy:
						EnemyHitEnemy(enemy, static_cast<Enemy*>(obj1));
						break;
					case GOT_Bullet:
						{
							Bullet* bullet = static_cast<Bullet*>(obj1);
							if(bullet->GetOwner()->GetObjectType() == GOT_Player)
							{
								WeaponHitEnemy(bullet, enemy);
							}
						}
						break;
					case  GOT_Missile:
						WeaponHitEnemy(static_cast<Missile*>(obj1), enemy);
						break;
					}
				}

				if(t0 == GOT_Bullet)
				{
					Bullet* bullet = static_cast<Bullet*>(obj0);
					switch (t1)
					{
					case GOT_Player:
						{
							if (bullet->GetOwner()->GetObjectType() == GOT_Enemy)
							{
								WeaponHitPlayerShip(bullet, static_cast<Player*>(obj1));
							}
						}
						break;
					case GOT_Enemy:
						{
							if (bullet->GetOwner()->GetObjectType() == GOT_Player)
							{
								WeaponHitEnemy(bullet, static_cast<Enemy*>(obj1));
							}
						}
						break;
					}
				}

				if(t0 == GOT_Missile)
				{
					Missile* bullet = static_cast<Missile*>(obj0);
					switch (t1)
					{
					case GOT_Player:
						{
							if (bullet->GetOwner()->GetObjectType() == GOT_Enemy)
							{
								WeaponHitPlayerShip(bullet, static_cast<Player*>(obj1));
							}
						}
						break;
					case GOT_Enemy:
						{
							if (bullet->GetOwner()->GetObjectType() == GOT_Player)
							{
								WeaponHitEnemy(bullet, static_cast<Enemy*>(obj1));
							}
						}
						break;
					}
				}

			}
		}


	}
	return true;
}





//////////////////////////////////////////////////////////////////////////





