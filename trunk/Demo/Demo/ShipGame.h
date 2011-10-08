/*
-----------------------------------------------------------------------------
Filename:    BasicTutorial3.h
-----------------------------------------------------------------------------
 
This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __BasicTutorial3_h_
#define __BasicTutorial3_h_
 
#include "Common.h"
#include "BaseApplication.h"


class GameLevel;
class Player;
class WeaponManager;
 
class ShipGame : public BaseApplication, public Ogre::Singleton<ShipGame>
{
	
private:
	Player* mGamePlayer;
	GameLevel* mLevel;
	WeaponManager*	mWeaponCreation;
	GameState*	mGameState;

public:
    ShipGame(void);
    virtual ~ShipGame(void);

	SceneManager*	GetSceneManager()	 { return mSceneMgr; }
	Camera*			GetCamera()			 { return mCamera;   }
	Root*			GetRoot()			 { return mRoot; }
	RenderWindow*	GetRenderWindow()	 { return mWindow; }
	GameState*		GetGameState()       { return mGameState; }

	OIS::Keyboard*		GetKeyboard()		 { return mKeyboard; }
	OIS::Mouse*			GetMouse()			 {	return mMouse; }

	Player*				GetPlayer();
	GameLevel&			GetGameLevel();
	
	void Startup();
	void Shutdown();


protected:
    virtual void createScene(void);
    virtual void createFrameListener(void);
    virtual void destroyScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool setup();

public:
	OGRE_SINGLETON_DECLARE(ShipGame);
	static void Initialize()
	{
		new ShipGame();
	}
	static void Finalize()
	{
		SAFE_DELETE(ms_Singleton)
	}
};
 
#endif // #ifndef __BasicTutorial3_h_