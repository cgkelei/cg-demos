#ifndef _Vector4__H
#define _Vector4__H

#include <math.h>

struct Vector4
{
	float X, Y, Z, W;
	
	Vector4(float fX, float fY, float fZ, float fW)
		: X(fX), Y(fY), Z(fZ), W(fW) { } 

	void operator += (const float s)	{ X += s; Y += s; Z += s; W += s; };
	void operator += (const Vector4 &v) { X += v.X; Y += v.Y; Z += v.Z; W += v.W; };
	void operator -= (const float s)	{ X -= s; Y -= s; Z -= s; W -= s; };
	void operator -= (const Vector4 &v) { X -= v.X; Y -= v.Y; Z -= v.Z; W -= v.W; };
	void operator *= (const float s)	{ X *= s; Y *= s; Z *= s; W *= s; };
	void operator *= (const Vector4 &v) { X *= v.X; Y *= v.Y; Z *= v.Z; W *= v.W; };
	void operator /= (const float s)    { X /= s; Y /= s; Z /= s; W /= s; };
	void operator /= (const Vector4 &v) { X /= v.X; Y /= v.Y; Z /= v.Z; W /= v.W; };
	
	Vector4 operator -(const Vector4& v) { return Vector4(X-v.X, Y-v.Y, Z-v.Z, W-v.W);}
	Vector4 operator +(const Vector4& v) { return Vector4(X+v.X, Y+v.Y, Z+v.Z, W+v.W);}
};

#endif