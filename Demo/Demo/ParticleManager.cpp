#include "ParticleManager.h"
#include "ShipGame.h"
#include "Utilities.h"

OGRE_SINGLETON_IMPLEMNET(ParticleManager)

ParticleManager::ParticleManager(void)
{
	mSceneMgr = ShipGame::getSingleton().GetSceneManager();
}


ParticleManager::~ParticleManager(void)
{
}

void ParticleManager::Explosion( Vector3 worldPos )
{
	/*Ogre::ParticleSystem* particleSystem = mSceneMgr->createParticleSystem(
	Utils::GetUniqueName("Explosion"), "PartilceBomb");*/



	// attach the particle system to a scene node
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(worldPos);
	node->attachObject(
		mSceneMgr->createEntity(Utils::GetUniqueName("hEAD"), "ogrehead.mesh"));
	node->scale(5,5,5);
}
