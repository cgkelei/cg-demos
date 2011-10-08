#include "Player.h"
#include "GameLevel.h"
#include "GameConstants.h"
#include "PhysicsEngine.h"
#include "Missile.h"
#include "Bullet.h"
#include "WeaponFactory.h"
#include "Utilities.h"

//////////////////////////////////////////////////////////////////////////

Player::Player(SceneManager* sceneMgr)
   : mName("Player"),
	mSceneMgr(sceneMgr),
	mWheelDegree(0.0f),
	mPitch(0.0f),
	mThrottle(0),
	mMissileLevel(WL_One),
	mBulletLevel(WL_One),
	mMissileCoolDownTime(-1),
	mBulletCoolDownTime(-1)
{

	// 加载飞机模型
	mSpaceShipEntity = mSceneMgr->createEntity(mName, GameConstants::PlayerMeshName); 
	mPlayerSceneNode = Utils::GetSingleton<GameLevel>().GetMainNode()->createChildSceneNode(Ogre::Vector3(0.0f, 1000.0f, 0.0f));
	mPlayerSceneNode->attachObject(mSpaceShipEntity);

	// 加载导弹
	Ogre::SceneNode* rightMissileNode = mPlayerSceneNode->createChildSceneNode("RightMissile", Ogre::Vector3(-50, -7.5f, -10));
	rightMissileNode->attachObject(sceneMgr->createEntity("RightMissile", GameConstants::F16MissileMeshName));
	rightMissileNode->pitch(Ogre::Radian(Ogre::Degree(90)));

	Ogre::SceneNode* leftMissileNode = mPlayerSceneNode->createChildSceneNode("LeftMissile", Ogre::Vector3(37, -7.5f, -10));
	leftMissileNode->attachObject(sceneMgr->createEntity("LeftMissile", GameConstants::F16MissileMeshName));
	leftMissileNode->pitch(Ogre::Radian(Ogre::Degree(90)));

	// 创建飞机尾部推进器粒子
	ParticleSystem* thrusters =sceneMgr->createParticleSystem(25);
	thrusters ->setMaterialName("Examples/Flare");
	thrusters ->setDefaultDimensions(25, 25);

	// create two emitters for our thruster particle system
	for (unsigned int i = 0; i < 2; i++)
	{
		ParticleEmitter* emitter = thrusters ->addEmitter("Point");  // add a point emitter

		// set the emitter properties
		emitter->setAngle(Degree(3));
		emitter->setTimeToLive(0.1f);
		emitter->setEmissionRate(1000);
		emitter->setParticleVelocity(100);
		emitter->setDirection(Vector3::NEGATIVE_UNIT_Z);
		emitter->setColour(ColourValue::White, ColourValue::Red);
		emitter->setPosition(Vector3(i == 0 ? 5.7f : -18, 0, 0));
	}

	// 绑定粒子到飞机尾部
	mPlayerSceneNode->createChildSceneNode(Vector3(0, 6.5f, -67))->attachObject(thrusters);

	mPosition = mPlayerSceneNode->getPosition();
	mOrientation = mPlayerSceneNode->getOrientation();


	// Feed Bullet
	Vector3 size = Vector3::ZERO;	// size of the box

	// we need the bounding box of the box to be able to set the size of the Bullet-box
	AxisAlignedBox boundingBox = mSpaceShipEntity->getBoundingBox();
	size = boundingBox.getSize(); 
	size /= 2.0f; // only the half needed
	size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size

	// Create bullet shape
	mShape = new BoxCollisionShape(size);

	// and the Bullet rigid body
	mRigidbody = new RigidBody(mName, Utils::GetSingleton<PhysicsEngine>().GetDynamicsWorld());

	// Set Shape
	mRigidbody->setShape(mPlayerSceneNode,mShape, mRestitution, mFriction, mMass, mPosition, mOrientation);

	// Set user pointer
	mRigidbody->getBulletRigidBody()->setUserPointer(this);

	mRigidbody->disableDeactivation();
	mRigidbody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK |
		mRigidbody->getBulletRigidBody()->getCollisionFlags());
	mRigidbody->setKinematicObject(true);
	mRigidbody->enableActiveState();

}


Player::~Player(void)
{
	mPlayerSceneNode->detachAllObjects();
	ShipGame::getSingleton().GetGameLevel().GetMainNode()->removeAndDestroyChild(mPlayerSceneNode->getName());
	ShipGame::getSingleton().GetSceneManager()->destroyEntity(mSpaceShipEntity);

	mSpaceShipEntity = NULL;
	mPlayerSceneNode = NULL;

	SAFE_DELETE(mShape);
	SAFE_DELETE(mRigidbody);
}


void Player::Update( float timeSinceLastFrame )
{
	if(ShipGame::getSingletonPtr()->GetKeyboard()->isKeyDown(OIS::KC_S))
	{
		//GameConstants::PlayerShipSpeed = 0;
	}

	if(ShipGame::getSingletonPtr()->GetKeyboard()->isKeyDown(OIS::KC_A))
	{
		// 处理左转弯
		mWheelDegree -= timeSinceLastFrame * GameConstants::PlayerWheelSpeed ; 
		
	}
	if(ShipGame::getSingletonPtr()->GetKeyboard()->isKeyDown(OIS::KC_D))
	{
		// 处理右转弯
		//mIsWheeling = true;
		mWheelDegree += timeSinceLastFrame * GameConstants::PlayerWheelSpeed ; 
		
	}

	if(ShipGame::getSingletonPtr()->GetKeyboard()->isKeyDown(OIS::KC_W))
	{
		mPitch += timeSinceLastFrame * GameConstants::PlayerClimbSpeed ; 
		//mThrottle += evt.timeSinceLastFrame * 90;
	}
	if(ShipGame::getSingletonPtr()->GetKeyboard()->isKeyDown(OIS::KC_S))
	{
		mPitch -= timeSinceLastFrame * GameConstants::PlayerClimbSpeed ; 
		//mThrottle -= evt.timeSinceLastFrame * 90;
	}
	
	mWheelDegree *= (1-timeSinceLastFrame*2);
	mPitch *= (1-timeSinceLastFrame*3);

	
	mOrientation.ToRotationMatrix(mWorldTransform);
	
	//Matrix3 graphicsWorldTransform = mOrientation;

	Matrix3 mod;
	mod.FromEulerAnglesXYZ(mPitch,-mWheelDegree*0.6f,mWheelDegree);  // ruan
	//graphicsWorldTransform = graphicsWorldTransform * mod;
	mWorldTransform = mWorldTransform * mod;  // ruan

	//mPlayerSceneNode->setOrientation(graphicsWorldTransform);
	mPlayerSceneNode->setOrientation(mWorldTransform);

	{
		Matrix3 rotYChange;
		rotYChange.FromAxisAngle(Vector3::UNIT_Y, -mWheelDegree*0.6f * timeSinceLastFrame);
		mOrientation = mOrientation * rotYChange;
	}


	//Vector3 direction = graphicsWorldTransform * Vector3::UNIT_Z;
	
	mDirection = mWorldTransform * Vector3::UNIT_Z; 
	mPosition += mDirection * GameConstants::PlayerShipSpeed * timeSinceLastFrame;

	mPlayerSceneNode->setPosition(mPosition);

	/*{
		btTransform trans;
		m_body->getMotionState()->getWorldTransform(trans);

		mPosition = Ogre::Vector3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		mOrientation = Ogre::Quaternion(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
		mPlayerSceneNode->setPosition(mPosition);
		mPlayerSceneNode->setOrientation(mOrientation);

	}*/
	/*Ogre::TerrainGroup::TerrainList resule;
	resule.clear();*/

	/*Ogre::Vector3 currPos = mPlayerSceneNode->getPosition();

	ShipGame::getSingleton().GetGameLevel().GetTerrainGroup()->boxIntersects(
		mSpaceShipEntity->getWorldBoundingBox(true), &resule );

	static int i = 0;
	if( resule.size() )
	{
		i++;
	}*/

	mRigidbody->setOrientation(mPlayerSceneNode->getOrientation());
	mRigidbody->setPosition(mPlayerSceneNode->getPosition());

	ProcessWeaponFireProcess(timeSinceLastFrame);
}

void Player::ProcessWeaponFireProcess(Real timeSinceLastFrame)
{
	 static bool LeftMouseDown = false;     // If a mouse button is depressed
	 static bool RightMouseDown = false;     // If a mouse button is depressed

	 Ogre::Matrix3 rotateMatrix;
	 mPlayerSceneNode->getOrientation().ToRotationMatrix(rotateMatrix);


	// 发导弹
	bool currRightDown = ShipGame::getSingleton().GetMouse()->getMouseState().buttonDown(OIS::MB_Right);
	if ( currRightDown && (! RightMouseDown) && mMissileCoolDownTime <= 0)
	{
		mWhichMissile = !mWhichMissile;

		Vector3 position = mPlayerSceneNode->getPosition() +
			mPlayerSceneNode->getOrientation() * Ogre::Vector3(mWhichMissile ? -50.0f : 37.0f, -7.5f, -10.0f);

		WeaponManager::getSingleton().CreateMissile(this, WL_One)->Fire(position, mDirection);
	}
	RightMouseDown = currRightDown;

	// 发子弹
	bool currLeftDown = ShipGame::getSingleton().GetMouse()->getMouseState().buttonDown(OIS::MB_Left);
	if (currLeftDown &&(! LeftMouseDown) && mBulletCoolDownTime <= 0)
	{
		WeaponManager::getSingleton().CreateBullet(this, mBulletLevel)
			->Fire(mPlayerSceneNode->getPosition(), mDirection);
	}
	LeftMouseDown = currLeftDown;

	mMissileCoolDownTime -= timeSinceLastFrame;
	mBulletCoolDownTime -= timeSinceLastFrame;
}


Ogre::Vector3 Player::GetDirection() const
{
	Matrix3 matrix;
	mPlayerSceneNode->getOrientation().ToRotationMatrix(matrix);
	return matrix.GetColumn(2);
}

GameObjectType Player::GetObjectType() const
{
	return GOT_Player;
}



