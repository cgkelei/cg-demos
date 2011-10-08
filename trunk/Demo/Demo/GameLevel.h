#pragma once
#ifndef GameLevel_h__
#define GameLevel_h__

//  [9/23/2011 hust_ruan]

#include "Common.h"
#include "ShipGameFrameListener.h"
#include "ChaseCameraControler.h"
#include "DotSceneLoader.h"

class GameLevel : Singleton<GameLevel>
{
public:
	GameLevel(void);
	virtual ~GameLevel(void);

	void Startup(const Ogre::String& scene);
	void Shutdown();

	Ogre::SceneNode*    GetMainNode() const;
	void				SetLevelLength(float length);
	
	void Update(Ogre::Real timeSinceLastFrame);
		
	void InitialiseVariables();

private:
	
	
	GameTerrain* mGameTerrain;
	DotSceneLoader* mDotSceneLoader;

	Ogre::SceneNode*     mMainNode; 

	// Chase Camera Controler
	ChaseCameraControler*		 mChaseCameraSpring;

	float				 mLevelLength;
	float				 mCameraStartZ;


public:
	OGRE_SINGLETON_DECLARE(GameLevel);
	static void Initialize()
	{
		new GameLevel();
	}
	static void Finalize()
	{
		SAFE_DELETE(ms_Singleton)
	}
};



#endif 