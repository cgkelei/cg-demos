#include "ShipGame.h"
#include "GameLevel.h"
#include "Player.h"
#include "GameState.h"
#include "WeaponFactory.h"
#include "PhysicsEngine.h"
#include "ParticleManager.h"

OGRE_SINGLETON_IMPLEMNET(ShipGame)

//-------------------------------------------------------------------------------------
ShipGame::ShipGame(void)
{

}
//-------------------------------------------------------------------------------------
ShipGame::~ShipGame(void)
{
}
//-------------------------------------------------------------------------------------
void ShipGame::destroyScene(void)
{
   
}

//-------------------------------------------------------------------------------------
void ShipGame::createScene(void)
{
    mCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
    mCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
    mCamera->setNearClipDistance(0.1f);
    mCamera->setFarClipDistance(50000);
 
    if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        mCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
 
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
 
 
    Ogre::Plane plane;
    plane.d = 100;
    plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
 
   // mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 500);
    mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 20, true, 0.5, 150, 150);
}
//-------------------------------------------------------------------------------------
void ShipGame::createFrameListener(void)
{
    BaseApplication::createFrameListener();
}
//-------------------------------------------------------------------------------------
bool ShipGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
 
    return ret;
}

bool ShipGame::frameStarted( const Ogre::FrameEvent& evt ) 
{
	bool ret = BaseApplication::frameStarted(evt);

	return mGameState->frameStarted(evt);
}

bool ShipGame::setup()
{
	bool ret = BaseApplication::setup();

	mGameState = new GameState();
	mWeaponCreation = new WeaponManager();
	
	ParticleManager::Initialize();
	GameLevel::Initialize();
	PhysicsEngine::Initialize(mSceneMgr, Vector3(0,-9.81f,0), // gravity vector for Bullet
		AxisAlignedBox (Ogre::Vector3 (-10000, -10000, -10000), //aligned box for Bullet
		Ogre::Vector3 (10000,  10000,  10000)));

	GameLevel::getSingleton().Startup("");
	mGamePlayer = new Player(mSceneMgr);
	

	return ret;
}

Player* ShipGame::GetPlayer()
{
	return mGamePlayer;
}

GameLevel& ShipGame::GetGameLevel()
{
	return *mLevel;
}


 
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        ShipGame* app = new ShipGame();
 
        try {
            app->go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif