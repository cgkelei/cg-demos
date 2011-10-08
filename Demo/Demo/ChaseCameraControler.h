#pragma once

#include <OgreVector3.h>

class ChaseCameraControler 
{ 
public:

	/// <summary>
	/// Position of camera in world space.
	/// </summary>
	Ogre::Vector3 mCameraPosition;

	/// <summary>
	/// Desired camera position in world space.
	/// </summary>
	Ogre::Vector3 mDesiredPosition;


	/// <summary>
	/// Velocity of camera.
	/// </summary>
	Ogre::Vector3 mVelocity;


	/// <summary>
	/// Physics coefficient which controls the influence of the camera's position
	/// over the spring force. The stiffer the spring, the closer it will stay to
	/// the chased object.
	/// </summary>
	float mStiffness;

	/// <summary>
	/// Physics coefficient which approximates internal friction of the spring.
	/// Sufficient damping will prevent the spring from oscillating infinitely.
	/// </summary>
	float mDampening;

	/// <summary>
	/// Mass of the camera body. Heaver objects require stiffer springs with less
	/// damping to move at the same rate as lighter objects.
	/// </summary>
	float mMass;


public:
	ChaseCameraControler(void);
	~ChaseCameraControler(void);

	///<Sets up a camera spring
	void InitCameraSpring(Ogre::Vector3 camPos,Ogre::Vector3 offsetPos);

	///<Updates the camera position
	Ogre::Vector3 UpdateCameraSpring( Ogre::Vector3 offsetPos, Ogre::Real timeSinceLastFrame );

};

