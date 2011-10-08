#include "ChaseCameraControler.h"
#include <OgreFrameListener.h>

//---------------------------------------------------------------------
ChaseCameraControler::ChaseCameraControler(void)
	: mCameraPosition(0,0,0), mDesiredPosition(0,0,0),mVelocity(0,0,0)
{
	mStiffness = 6;
	mDampening = 6;
	mMass = 10;
}



ChaseCameraControler::~ChaseCameraControler(void)
{
}

void ChaseCameraControler::InitCameraSpring( Ogre::Vector3 camPos,Ogre::Vector3 offsetPos )
{
	mCameraPosition = camPos;
	mDesiredPosition = offsetPos;
}

Ogre::Vector3 ChaseCameraControler::UpdateCameraSpring( Ogre::Vector3 offsetPos, Ogre::Real timeSinceLastFrame )
{
	//Set the desired position, e.g. somewhere behind and above the camera
	mDesiredPosition = offsetPos;

	//Set the cameras current position
	//mCameraPosition = camPos;

	// Calculate spring force
	Ogre::Vector3 stretch = mCameraPosition - mDesiredPosition;
	Ogre::Vector3 force = -mStiffness * stretch - mDampening * mVelocity;

	// Apply acceleration
	Ogre::Vector3 acceleration = force * ( 1/ mMass );

	//Add the acceleration to the velocity
	mVelocity += acceleration;

	// Apply velocity
	mCameraPosition += timeSinceLastFrame * mVelocity;

	//Return new camera position
	return mCameraPosition;
}
