#pragma once

#include "Common.h"

class ParticleManager : public Singleton<ParticleManager>
{
public:
	ParticleManager();
	~ParticleManager(void);

	void Explosion(Vector3 worldPos);


	static void Initialize()
	{
		new ParticleManager();
	}

	static void Finalize()
	{
		SAFE_DELETE(ms_Singleton)
	}

public:
	OGRE_SINGLETON_DECLARE(ParticleManager);

	SceneManager* mSceneMgr;
};

