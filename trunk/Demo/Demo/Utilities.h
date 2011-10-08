#pragma once
#ifndef _Utilities__H
#define _Utilities__H

#include "Common.h"
#include "BulletCommon.h"

namespace Utils
{
	template <typename ObjectType>
	inline typename ObjectType&
    GetSingleton()
	{
		return ObjectType::getSingleton();
	}

	template <typename ObjectType>
	inline typename ObjectType*
	GetSingletonPtr()
	{
		return ObjectType::getSingletonPtr();
	}

	static inline String GetUniqueName(String prefix)
	{
		static int count = 0;
		return prefix + StringConverter::toString(count++);
	}

	static inline btVector3 ToBTVec3(const Ogre::Vector3& v)
	{
		return btVector3(v.x, v.y, v.z);
	};
	static inline Ogre::Vector3 ToOgreVector3(const btVector3& v)
	{
		return Ogre::Vector3(v.getX(), v.getY(), v.getZ());
	};

}


#endif // _Utilities__H