#pragma once
#ifndef _GameState__H
#define _GameState__H

#include "Common.h"

class GameState //: public FrameListener
{
public:
	typedef std::vector<Enemy*> EnemyList;
	typedef std::vector<Missile*> MissileList;
	typedef std::vector<Bullet*> BulletList;

public:
	GameState(void);
	~GameState(void);

	bool frameStarted(const FrameEvent& evt);

	EnemyList& GetEnemyList();
	MissileList& GetMissileList();
	BulletList& GetBulletList();

	void AddEnemy(Enemy* enemy);
	void RemoveEnemy(Enemy* enemy);
	void AddMissile(Missile* missile);
	void RemoveMissile(Missile* missile);
	void AddBullet(Bullet* bullet);
	void RemoveBullet(Bullet* bullet);
	
	void UpdateEnemies(Real timeSinceLastFrame);
	void UpdateMissiles(Real timeSinceLastFrame);
	void UpdateBullets(Real timeSinceLastFrame);
	void UpdatePlayer(Real timeSinceLastFrame);

private:
	
	EnemyList mEnemies;
	MissileList mMissiles;
	BulletList mBullets;
	Player* mPlayer;

	unsigned int mCurrentLevel;


};


#endif // _GameState__H

