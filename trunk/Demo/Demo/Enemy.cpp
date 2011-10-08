#include "Enemy.h"
#include "PhysicsEngine.h"
#include "Utilities.h"
#include "BulletCommon.h"
#include "GameLevel.h"

Enemy::Enemy( SceneManager* sceneMrg, const String& meshName, const Vector3& startPos, const Vector3& scale, const Quaternion& startQuaternion , const String& material)
	: mSceneMgr(sceneMrg), mPosition(startPos), mOrientation(startQuaternion), mMeshName(meshName)
{
	mEnemyEntity = mSceneMgr->createEntity(mName, mMeshName);
	mEnemyEntity->setCastShadows(true);
	
	if(!material.empty())
		mEnemyEntity->setMaterialName(material);

	mEnemyNode = Utils::GetSingleton<GameLevel>().GetMainNode()->createChildSceneNode(mPosition);
	mEnemyNode->setOrientation(mOrientation);
	mEnemyNode->attachObject(mEnemyEntity);

	// Feed Bullet
	Vector3 size = Vector3::ZERO;	// size of the box

	// we need the bounding box of the box to be able to set the size of the Bullet-box
	AxisAlignedBox boundingBox = mEnemyEntity->getBoundingBox();
	size = boundingBox.getSize(); 
	size /= 2.0f; // only the half needed
	size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size

	// Create bullet shape
	mShape = new BoxCollisionShape(size);

	// and the Bullet rigid body
	mRigidbody = new RigidBody(mName, Utils::GetSingleton<PhysicsEngine>().GetDynamicsWorld());

	// Set Shape
	mRigidbody->setShape(mEnemyNode,mShape, mRestitution, mFriction, mMass, mPosition, mOrientation);

	// Set user pointer
	mRigidbody->getBulletRigidBody()->setUserPointer(this);

	mRigidbody->disableDeactivation();
	mRigidbody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK |
		mRigidbody->getBulletRigidBody()->getCollisionFlags());
	mRigidbody->enableActiveState();

	mIsDead = false;

}

//Enemy::Enemy(SceneManager* sceneMgr, NameValuePairList creationInfo)
//	
//{
//	
//	mEnemyEntity = mSceneMgr->createEntity(mName, mMeshName);
//	mEnemyEntity->setCastShadows(true);
//
//	mEnemyNode = Utils::GetSingleton<GameLevel>().GetMainNode()->createChildSceneNode(); 
//	mEnemyNode->attachObject(mEnemyEntity);
//
//	// Feed Bullet
//	Vector3 size = Vector3::ZERO;	// size of the box
//
//	// we need the bounding box of the box to be able to set the size of the Bullet-box
//	AxisAlignedBox boundingBox = mEnemyEntity->getBoundingBox();
//	size = boundingBox.getSize(); 
//	size /= 2.0f; // only the half needed
//	size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
//
//	// Create bullet shape
//	mShape = new BoxCollisionShape(size);
//	
//	// and the Bullet rigid body
//	mRigidbody = new RigidBody(mName, Utils::GetSingleton<PhysicsEngine>().GetDynamicsWorld());
//	
//	// Set Shape
//	mRigidbody->setShape(mEnemyNode,mShape, mRestitution, mFriction, mMass, mPosition, mOrientation);
//	
//	// Set user pointer
//	mRigidbody->getBulletRigidBody()->setUserPointer(this);
//	
//	mRigidbody->disableDeactivation();
//	mRigidbody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK |
//		mRigidbody->getBulletRigidBody()->getCollisionFlags());
//	mRigidbody->enableActiveState();
//}




Enemy::~Enemy(void)
{
	mEnemyNode->detachAllObjects();
	ShipGame::getSingleton().GetGameLevel().GetMainNode()->removeAndDestroyChild(mEnemyNode->getName());
	ShipGame::getSingleton().GetSceneManager()->destroyEntity(mEnemyEntity);

	mEnemyNode = NULL;
	mEnemyEntity = NULL;

	SAFE_DELETE(mShape);
	SAFE_DELETE(mRigidbody);
}

GameObjectType Enemy::GetObjectType() const
{
	return GOT_Enemy;
}

void Enemy::Update( Real timeSinceLastFrame )
{
	if(!mIsDead)
	{
		mRigidbody->setOrientation(mEnemyNode->getOrientation());
		mRigidbody->setPosition(mEnemyNode->getPosition());
	}
}



