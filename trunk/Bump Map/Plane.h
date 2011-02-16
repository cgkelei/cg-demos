#ifndef _Plane__H
#define _Plane__H

#include "Vector3.h"
#include "Vector4.h"

class  Plane
{
public:
	Plane() :Normal(0.0f, 1.0f, 0.0f), D(0.0f) { }
	Plane(const Vector3& normal, const Vector3& mPoint);
	Plane(const Vector4& plane);

	void  SetPlane(const Vector4& plane);
	float GetDistanceTo(const Vector3& point) const ;
	void  Normalize();

	/*in the front >0
	  in the plane == 0 
	  in the back  <0
	 */
	float ClassifyPoint(float fX, float fY, float fZ);

private:
	void RecalculateDistance(const Vector3& mPoint);

public:
	Vector3 Normal;
	float	D;
};

inline Plane::Plane(const Vector3& normal, const Vector3& mPoint)
{
	Normal = normal;
	RecalculateDistance(mPoint);
}


inline Plane::Plane(const Vector4& plane)
{
	SetPlane(plane);
}

inline void Plane::SetPlane(const Vector4& plane)
{
	Normal = Vector3(plane.X, plane.Y, plane.Z);
	D = plane.W;
}

void Plane::Normalize()
{
	D /= Normal.getLength();
	Normal.normalize();
}


void Plane::RecalculateDistance(const Vector3& mPoint)
{
	D = - mPoint.dotProduct(Normal);
}

float Plane::GetDistanceTo(const Vector3& point) const
{
	 return Normal.dotProduct(point) + D;
}

float Plane::ClassifyPoint( float fX, float fY, float fZ )
{
	float d;
	d =Normal.X * fX + Normal.Y * fY + Normal.Z * fZ + D;
	return  d;
}

#endif