#pragma once
#ifndef _GameTerrain__H
#define _GameTerrain__H

#include "Common.h"

class GameTerrain
{
public:
	GameTerrain(SceneManager* sceneMgr, const String heightmap);
	~GameTerrain(void);

public: 
	TerrainGroup* GetTerrainGroup();
	TerrainGlobalOptions* GetTerrainGlobals();
	void LoadGameTerrain();
	void UnloadGameTerrain();
	void SaveGameTerrain(bool onlyIfModified);
	void FeedPhysicsEngine(PhysicsEngine* physicsEngine);

private:
	void DefineTerrain(long x, long y);
	void InitBlendMaps(Ogre::Terrain* terrain);
	void ConfigureTerrainDefaults(Ogre::Light* light);
	void GetTerrainImage(bool flipX, bool flipY, Ogre::Image& img);

private:

	SceneManager*	mSceneMgr;

	String	mHeighetmapFilename;
	float* mHeightFieldData;			// feed for bullet engine

	TerrainGlobalOptions* mTerrainGlobalOptions;
	TerrainGroup* mTerrainGroup;
	bool mTerrainsImported;

	

};

#endif