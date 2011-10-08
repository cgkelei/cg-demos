#include "GameTerrain.h"
#include "PhysicsEngine.h"

#define TERRAIN_FILE_PREFIX String("ShipGame")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513

GameTerrain::GameTerrain( SceneManager* sceneMgr, const String heightmap )
	: mSceneMgr(sceneMgr), 
	  mHeighetmapFilename(heightmap),
	  mHeightFieldData(NULL),
	  mTerrainGlobalOptions(NULL),
	  mTerrainGroup(NULL),
	  mTerrainsImported(false)
{

}


GameTerrain::~GameTerrain(void)
{
	
}

TerrainGroup* GameTerrain::GetTerrainGroup()
{
	return mTerrainGroup;
}

TerrainGlobalOptions* GameTerrain::GetTerrainGlobals()
{
	return mTerrainGlobalOptions;
}

void GameTerrain::LoadGameTerrain()
{
	Vector3 lightdir(0.55f, -0.3f, 0.75f);
	lightdir.normalise();

	Light* light = mSceneMgr->createLight("tstLight");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

	mTerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();

	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	ConfigureTerrainDefaults(light);

	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			DefineTerrain(x, y);

	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);

	if (mTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			InitBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();
}

void GameTerrain::UnloadGameTerrain()
{
	SAFE_DELETE(mTerrainGroup);
	SAFE_DELETE(mTerrainGlobalOptions);
	SAFE_DELETE(mHeightFieldData);
}


void GameTerrain::SaveGameTerrain( bool onlyIfModified )
{
	mTerrainGroup->saveAllTerrains(onlyIfModified);
}


void GameTerrain::FeedPhysicsEngine(PhysicsEngine* physicsEngine)
{
	const unsigned int pageSize = TERRAIN_SIZE;
	const float worldSize = TERRAIN_WORLD_SIZE;
	const float maxHeight = 600.0f;

	mHeightFieldData = new float [pageSize*pageSize];
	Ogre::Image terrainHeightMap;
	terrainHeightMap.load(mHeighetmapFilename, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	for(unsigned y = 0; y < pageSize; ++y)
	{
		for(unsigned x = 0; x < pageSize; ++x)
		{
			Ogre::ColourValue color = terrainHeightMap.getColourAt(x, y, 0);
			mHeightFieldData[x + y * pageSize] = color.r;
		}
	}

	physicsEngine->PutTerrainData(pageSize, worldSize, mHeightFieldData, maxHeight);
}



void GameTerrain::GetTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
}

void GameTerrain::DefineTerrain( long x, long y )
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
	{
		mTerrainGroup->defineTerrain(x, y);
	}
	else
	{
		Ogre::Image img;
		GetTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainsImported = true;
	}
}

void GameTerrain::InitBlendMaps( Ogre::Terrain* terrain )
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void GameTerrain::ConfigureTerrainDefaults( Ogre::Light* light )
{
	// Configure global
	mTerrainGlobalOptions->setMaxPixelError(0);
	// testing composite map
	mTerrainGlobalOptions->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobalOptions->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobalOptions->setCompositeMapDiffuse(light->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}



