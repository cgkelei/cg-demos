#include "GameLevel.h"
#include "ShipGame.h"
#include "Player.h"
#include "PhysicsEngine.h"
#include "GameTerrain.h"
#include "Utilities.h"

OGRE_SINGLETON_IMPLEMNET(GameLevel)


static const char* LEVEL_GROUP_NAME = "General";



GameLevel::GameLevel(void)
{
	InitialiseVariables();

	// new chase camera control
	mChaseCameraSpring = new ChaseCameraControler();
	mDotSceneLoader = new DotSceneLoader();
}


GameLevel::~GameLevel(void)
{
	SAFE_DELETE(mChaseCameraSpring);
	SAFE_DELETE(mGameTerrain);
	SAFE_DELETE(mDotSceneLoader);
}

void GameLevel::InitialiseVariables()
{
	mMainNode = NULL;
	mLevelLength = 0;
	mCameraStartZ = 0;

}

void GameLevel::Startup( const Ogre::String& scene )
{

	ShipGame* shipGame = Utils::GetSingletonPtr<ShipGame>();

	// Create scene main node, all entity attach to this node
	mMainNode = shipGame->GetSceneManager()->getRootSceneNode()->createChildSceneNode(Vector3::ZERO);

	//// Load Terrain
	mGameTerrain = new GameTerrain(shipGame->GetSceneManager(), "terrain.png"); //LoadTerrain();
	mGameTerrain->LoadGameTerrain();
	mGameTerrain->FeedPhysicsEngine(Utils::GetSingletonPtr<PhysicsEngine>());

	// Start up player
	//shipGame->GetPlayer()->Startup();
	
	mDotSceneLoader->parseDotScene("scene.xml","General", shipGame->GetSceneManager());
	// Set Chase Camera Initial Position
	const Ogre::Vector3& CameraPos = shipGame->GetCamera()->getPosition();
	mChaseCameraSpring->InitCameraSpring(CameraPos, CameraPos);
}

void GameLevel::Shutdown()
{

	//Utils::GetSingletonPtr<ShipGame>()->GetPlayer()->Shutdown();
	InitialiseVariables();
}

Ogre::SceneNode* GameLevel::GetMainNode() const
{
	return mMainNode;
}

void GameLevel::SetLevelLength( float length )
{
	mLevelLength = length;
}



void GameLevel::Update( Ogre::Real timeSinceLastFrame )
{
	const Ogre::Vector3& ChasePosition = ShipGame::getSingleton().GetPlayer()->GetPlayerSceneNode()->getPosition();
		
	Matrix3 trans;
	ShipGame::getSingleton().GetPlayer()->GetPlayerSceneNode()->getOrientation().ToRotationMatrix(trans);
		
	// Calculate desired camera properties in world space
	Ogre::Vector3 desiredPosition = ChasePosition + trans * Ogre::Vector3(0, 50, 0);	
	Ogre::Vector3 pos =  mChaseCameraSpring->UpdateCameraSpring(desiredPosition, timeSinceLastFrame);
	Vector3 offset = trans.GetColumn(2) * 180;
	ShipGame::getSingleton().GetCamera()->setPosition(pos+offset);
	ShipGame::getSingleton().GetCamera()->lookAt(ChasePosition);

}
