#ifndef _Vector3__H
#define _Vector3__H

#include <math.h>

struct Vector3
{
	float X, Y, Z;

public:
	Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
	Vector3(float x_, float y_, float z_) : X(x_), Y(y_), Z(z_) {}
	Vector3(float value[3]) : X(value[0]), Y(value[1]), Z(value[2]){}

	Vector3  operator- (const Vector3& a) const { return Vector3(X-a.X, Y-a.Y, Z-a.Z);}
	Vector3  operator+ (const Vector3& a) const { return Vector3(X+a.X, Y+a.Y, Z+a.Z);}
	Vector3& operator*= (float v) { X*=v; Y*=v; Z*=v;  return *this;}
	Vector3  operator* (float v) { return Vector3(X*v, Y*v, Z*v); }

	Vector3 crossProduct(const Vector3& v) const
	{
		Vector3 cross;
		cross.X = Y * v.Z - Z * v.Y;
		cross.Y = Z * v.X - X * v.Z;
		cross.Z = X * v.Y - Y * v.X;
		return cross;
	}

	float dotProduct(const Vector3& value) const 
	{
		return X * value.X + Y * value.Y + Z * value.Z;
	}


	Vector3& normalize();
	float    getLength()		{ return sqrtf(X*X+Y*Y+Z*Z); };
	float    getLengthSQ()		{ return X*X+Y*Y+Z*Z; }

};

inline Vector3& Vector3::normalize()
{
	float recLength = 1.0f / sqrtf(X*X+Y*Y+Z*Z);
	X *= recLength; Y *= recLength; Z *= recLength;
	return * this;
}

#endif