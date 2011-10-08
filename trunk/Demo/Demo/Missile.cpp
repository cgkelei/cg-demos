#include "Missile.h"
#include "GameConstants.h"
#include "ShipGame.h"
#include "BulletCommon.h"
#include "PhysicsEngine.h"
#include "Utilities.h"

Missile::Missile(SceneManager* sceneMgr, ShipGameObject* owner, WeaponType type, WeaponLevel level, bool isLeftMissile)
	: Weapon(sceneMgr, owner, type, level), mOrientation(), mRigidbody(0), mIsHit(false),
	mThrusters(0), mThrustersNode(0), mIsEplosion(false)
{
	mName = Utils::GetUniqueName("Missile");
	mVelocity = GameConstants::PlayerShipSpeed * 0.4f ;
	if(isLeftMissile)
		mInitialMoment = Vector3(45,-45,0);	
	else
		mInitialMoment = Vector3(-45,-45,0);	

	if(mWeaponOwner->GetObjectType() == GOT_Player)
	{
		mMissileEntity = sceneMgr->createEntity(GameConstants::F16MissileMeshName);
		mMissileEntity->setCastShadows(true);
	}
	else if(mWeaponOwner->GetObjectType() == GOT_Enemy)
	{
		//敌人不发导弹
		assert(false);
	}

	mWeaponSceneNode->attachObject(mMissileEntity);

	// Feed Bullet
	Vector3 size = Vector3::ZERO;	// size of the box

	// we need the bounding box of the box to be able to set the size of the Bullet-box
	AxisAlignedBox boundingBox = mMissileEntity->getBoundingBox();
	size = boundingBox.getSize(); 
	size /= 2.0f; // only the half needed
	size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size

	// Create bullet shape
	mShape = new BoxCollisionShape(size);
	
}


Missile::~Missile(void)
{
}


GameObjectType Missile::GetObjectType() const
{
	return GOT_Missile;
}

void Missile::Fire( const Ogre::Vector3& startPostion, Ogre::Vector3& direction )
{
	Weapon::Fire(startPostion, direction);

	// 创建导弹尾部推进器粒子
	mThrusters = mSceneMgr->createParticleSystem(25);
	mThrusters ->setMaterialName("Examples/Flare");
	mThrusters ->setDefaultDimensions(25, 25);

	ParticleEmitter* emitter = mThrusters ->addEmitter("Point");  // add a point emitter

	// set the emitter properties
	emitter->setAngle(Degree(3));
	emitter->setTimeToLive(0.1f);
	emitter->setEmissionRate(100);
	emitter->setParticleVelocity(250);
	emitter->setDirection(Vector3::NEGATIVE_UNIT_Z);
	emitter->setColour(ColourValue::White, ColourValue::Red);
	emitter->setPosition(Vector3(0, 0, 0));

	// 绑定粒子到飞机尾部
	mThrustersNode = mWeaponSceneNode->createChildSceneNode(Vector3(0, -40.0f, 5));
	mThrustersNode->attachObject(mThrusters);
	
	Vector3 tY = Vector3::UNIT_Y;
	Vector3 tX = tY.crossProduct(direction);

	tX.normalise();
	tY = direction.crossProduct(tX);

	Matrix3 orientation = Matrix3::IDENTITY;
	orientation.SetColumn(2, direction);
	orientation.SetColumn(1, tY);
	orientation.SetColumn(0, tX);

	Matrix3 baseRot;
	baseRot.FromAxisAngle(Vector3::UNIT_X, Ogre::Radian(Ogre::Math::HALF_PI));

	mWeaponSceneNode->setOrientation(orientation * baseRot);
	
	m_fireOrientation = orientation;

	mOrientation = mWeaponSceneNode->getOrientation();
	// and the Bullet rigid body
	mRigidbody = new RigidBody(mName, Utils::GetSingleton<PhysicsEngine>().GetDynamicsWorld());
	// Set Shape
	mRigidbody->setShape(mWeaponSceneNode,mShape, mRestitution, mFriction, mMass, mPosition, mOrientation);
	// Set user date
	mRigidbody->getBulletRigidBody()->setUserPointer(this);
	mRigidbody->disableDeactivation();
	mRigidbody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK |
		mRigidbody->getBulletRigidBody()->getCollisionFlags());
	mRigidbody->setKinematicObject(true);
	mRigidbody->enableActiveState();

}


void Missile::Update( Real timeSinceLastFrame )
{
	if(mIsHit == false)
	{
		mPosition += mVelocity * mFireDirection * timeSinceLastFrame;
		mWeaponSceneNode->setPosition(mPosition);

		mVelocity += GameConstants::MissleAcceleration * timeSinceLastFrame;

		if (mVelocity>GameConstants::MaxMissleVelocity)
		{
			mVelocity = GameConstants::MaxMissleVelocity;
		}

		mInitialMoment *= (1-timeSinceLastFrame);
		mPosition += (m_fireOrientation * mInitialMoment) * timeSinceLastFrame;

		if (mRigidbody)
		{
			mRigidbody->setPosition(mPosition);
		}
	}	
}



