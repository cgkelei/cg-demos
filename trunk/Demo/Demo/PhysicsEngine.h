#pragma once
#ifndef _PhysicsEngine__H
#define _PhysicsEngine__H

#include "Common.h"
#include "BulletCommon.h"


class PhysicsEngine : public Ogre::Singleton<PhysicsEngine>
{
public:
	PhysicsEngine(SceneManager* sceneManager, Vector3 &gravityVector, AxisAlignedBox &bounds);
	~PhysicsEngine(void);

	DynamicsWorld* GetDynamicsWorld();

	// ‘› ±≤ª”√
	void AddBody(RigidBody* body);
	void AddShape(CollisionShape* shape);

	void PutTerrainData(unsigned int pageSize, Real worldSize, float* data, 
		Real maxHeight, const Vector3& position = Vector3::ZERO);

	void Update(Real timeSinceLastFrame);

private:

	SceneManager* mSceneMgr;
		
	DynamicsWorld* mPhysicsWorld;  
	DebugDrawer* mDebugDrawer;

	typedef std::deque<RigidBody*>			BodyList;
	typedef BodyList::iterator					BodyListIter;
	typedef std::deque<CollisionShape*>		ShapeList;
	typedef ShapeList::iterator					ShapeListIter;

	BodyList   mBodies;
	ShapeList  mShapes;

	bool mIsShowDebug;


public:
	
	static void Initialize(SceneManager* sceneManager, Vector3 &gravityVector, AxisAlignedBox &bounds)
	{
		new PhysicsEngine(sceneManager, gravityVector, bounds);
	}
	static void Finalize()
	{
		SAFE_DELETE(ms_Singleton)
	}

	OGRE_SINGLETON_DECLARE(PhysicsEngine)
};

#endif
