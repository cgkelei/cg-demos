#include "GameState.h"
#include "ShipGame.h"
#include "Missile.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "PhysicsEngine.h"
#include "GameLevel.h"

GameState::GameState(void)
{
	//ShipGame::getSingleton().GetRoot()->addFrameListener(this);
}


GameState::~GameState(void)
{
}

GameState::EnemyList& GameState::GetEnemyList()
{
	return mEnemies;
}

GameState::MissileList& GameState::GetMissileList()
{
	return mMissiles;
}

GameState::BulletList& GameState::GetBulletList()
{
	return mBullets;
}

void GameState::AddEnemy( Enemy* enemy )
{
	assert(enemy != NULL);
	mEnemies.push_back(enemy);
}

void GameState::RemoveEnemy( Enemy* enemy )
{
	EnemyList::iterator iter = std::find(mEnemies.begin(),
		mEnemies.end(), enemy);

	if(iter != mEnemies.end())
	{
		mEnemies.erase(iter);
	}
}

void GameState::AddMissile( Missile* missile )
{
	assert(missile != NULL);
	mMissiles.push_back(missile);
}

void GameState::RemoveMissile( Missile* missile )
{
	MissileList::iterator iter = std::find(mMissiles.begin(),
		mMissiles.end(), missile);

	if(iter != mMissiles.end())
	{
		mMissiles.erase(iter);
	}
}

void GameState::AddBullet( Bullet* bullet )
{
	assert(bullet != NULL);
	mBullets.push_back(bullet);
}

void GameState::RemoveBullet( Bullet* bullet )
{
	BulletList::iterator iter = std::find(mBullets.begin(),
		mBullets.end(), bullet);

	if(iter != mBullets.end())
	{
		mBullets.erase(iter);
	}
}

void GameState::UpdateEnemies( Real timeSinceLastFrame )
{
	for (EnemyList::iterator iter = mEnemies.begin(); 
		iter != mEnemies.end(); ++iter)
	{
		(*iter)->Update(timeSinceLastFrame);
	}
}

void GameState::UpdateMissiles( Real timeSinceLastFrame )
{
	for (MissileList::iterator iter = mMissiles.begin(); 
		iter != mMissiles.end(); ++iter)
	{
		(*iter)->Update(timeSinceLastFrame);
	}
}


void GameState::UpdateBullets( Real timeSinceLastFrame )
{
	for (BulletList::iterator iter = mBullets.begin(); 
		iter != mBullets.end(); ++iter)
	{
		(*iter)->Update(timeSinceLastFrame);
	}
}


void GameState::UpdatePlayer( Real timeSinceLastFrame )
{
	ShipGame::getSingleton().GetPlayer()->Update(timeSinceLastFrame);
}

bool GameState::frameStarted( const FrameEvent& evt )
{
	PhysicsEngine::getSingleton().Update(evt.timeSinceLastFrame);
	UpdatePlayer(evt.timeSinceLastFrame);
	UpdateEnemies(evt.timeSinceLastFrame);
	UpdateMissiles(evt.timeSinceLastFrame);
	UpdateBullets(evt.timeSinceLastFrame);
	GameLevel::getSingleton().Update(evt.timeSinceLastFrame);
	return true;
}
