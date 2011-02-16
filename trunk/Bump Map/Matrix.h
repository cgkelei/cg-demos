#ifndef _Matrix4__H
#define _Matrix4__H

#include "Config.h"
#include "Vector3.h"
#include "Vector4.h"
#include <math.h>

//Row Order

class Matrix4
{
public:
	Matrix4(void) { MakeIdentity();}
	Matrix4(const Matrix4& mat) { *this = mat;}   // copy constructor
	
	Matrix4& operator= (const Matrix4& mat);
	Matrix4& operator*= (const Matrix4& mat);
	float&		 operator[] (int index) { return M[index]; }
	const float& operator[] (int index) const { return M[index]; }

public:
	Matrix4& MakeIdentity();
	Matrix4& BuildCameraLookAtMatrixRH(const Vector3& position, const Vector3& lookAt, const Vector3& upVec);
	Matrix4& BuildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);  
	Matrix4& BuildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);
	Matrix4& BuildTranslateMatrix(float x, float y, float z);
	Matrix4& BuildRotationXMatrix(float theta);
	Matrix4& BuildRotationYMatrix(float theta);
	Matrix4& BuildRotationZMatrix(float theta);
	Vector4  GetRowVector(int row) const { return Vector4(M[row*4], M[row*4+1], M[row*4+2], M[row*4+3]);}
	Vector4  GetColumnVector(int col) const { return Vector4(M[col], M[col+4], M[col+8], M[col+12]);}
	float*   GetMatrix() { return M;}

private:
	float M[16];
};


inline Matrix4& Matrix4::MakeIdentity()
{
	ZeroMemory(&M, 16*sizeof(float));
	M[0] = M[5] = M[10] = M[15] = 1.0f;
	return *this;
}

inline Matrix4& Matrix4::operator= (const Matrix4& mat)
{
	if (this == &mat)
		return *this;
	memcpy(M, mat.M, 16*sizeof(float));
	return *this;
}

inline Matrix4& Matrix4::operator*= (const Matrix4& mat)
{
	Matrix4 temp ( *this );

	M[0] = temp[0]*mat[0] + temp[1]*mat[4] + temp[2]*mat[8] + temp[3]*mat[12];
	M[1] = temp[0]*mat[1] + temp[1]*mat[5] + temp[2]*mat[9] + temp[3]*mat[13];
	M[2] = temp[0]*mat[2] + temp[1]*mat[6] + temp[2]*mat[10] + temp[3]*mat[14];
	M[3] = temp[0]*mat[3] + temp[1]*mat[7] + temp[2]*mat[11] + temp[3]*mat[15];

	M[4] = temp[4]*mat[0] + temp[5]*mat[4] + temp[6]*mat[8] + temp[7]*mat[12];
	M[5] = temp[4]*mat[1] + temp[5]*mat[5] + temp[6]*mat[9] + temp[7]*mat[13];
	M[6] = temp[4]*mat[2] + temp[5]*mat[6] + temp[6]*mat[10] + temp[7]*mat[14];
	M[7] = temp[4]*mat[3] + temp[5]*mat[7] + temp[6]*mat[11] + temp[7]*mat[15];

	M[8] = temp[8]*mat[0] + temp[9]*mat[4] + temp[10]*mat[8] + temp[11]*mat[12];
	M[9] = temp[8]*mat[1] + temp[9]*mat[5] + temp[10]*mat[9] + temp[11]*mat[13];
	M[10] = temp[8]*mat[2] + temp[9]*mat[6] + temp[10]*mat[10] + temp[11]*mat[14];
	M[11] = temp[8]*mat[3] + temp[9]*mat[7] + temp[10]*mat[11] + temp[11]*mat[15];

	M[12] = temp[12]*mat[0] + temp[13]*mat[4] + temp[14]*mat[8] + temp[15]*mat[12];
	M[13] = temp[12]*mat[1] + temp[13]*mat[5] + temp[14]*mat[9] + temp[15]*mat[13];
	M[14] = temp[12]*mat[2] + temp[13]*mat[6] + temp[14]*mat[10] + temp[15]*mat[14];
	M[15] = temp[12]*mat[3] + temp[13]*mat[7] + temp[14]*mat[11] + temp[15]*mat[15];
	
	return *this;
}

inline Matrix4& Matrix4::BuildTranslateMatrix(float xMov, float yMov, float zMov)
{
	M[0]= 1.0f;
	M[1]=M[2]=0.0f;
	M[3] = xMov;

	M[4] = M[6] = 0.f;
	M[5] = 1.0f;
	M[7] = yMov;

	M[8] = M[9] = 0.0f;
	M[10] = 1.0f;
	M[11] = zMov;

	M[12] = M[13] = M[14] = 0.0f;
	M[15] = 1.0f;

	return *this;
}

inline Matrix4& Matrix4::BuildRotationXMatrix(float theta)
{
	M[0] = 1.0f;
	M[1] = 0.0f;
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = cos(theta);
	M[6] = -sin(theta);
	M[7] = 0.0f;

	
	M[4] = sin(theta);
	M[5] = cos(theta);
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[4] = 0.0f;
	M[5] = 0.0f;
	M[6] = 0.0f;
	M[7] = 1.0f;

	return *this;
}

inline Matrix4& Matrix4::BuildRotationYMatrix(float theta)
{
	M[0] = cos(theta);
	M[1] = 0.0f;
	M[2] = sin(theta);
	M[3] = 0.0f;

	M[4] = 0.0f;
	M[5] = 1.0f;
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[4] = -sin(theta);
	M[5] = 0.0f;
	M[6] = cos(theta);
	M[7] = 0.0f;

	M[4] = 0.0f;
	M[5] = 0.0f;
	M[6] = 0.0f;
	M[7] = 1.0f;

	return *this;
}

inline Matrix4& Matrix4::BuildRotationZMatrix(float theta)
{
	M[0] = cos(theta);
	M[1] = -sin(theta);
	M[2] = 0.0f;
	M[3] = 0.0f;

	M[4] = sin(theta);
	M[5] = cos(theta);
	M[6] = 0.0f;
	M[7] = 0.0f;

	M[4] = 0.0f;
	M[5] = 0.0f;
	M[6] = 1.0f;
	M[7] = 0.0f;

	M[4] = 0.0f;
	M[5] = 0.0f;
	M[6] = 0.0f;
	M[7] = 1.0f;

	return *this;
}

inline Matrix4& Matrix4::BuildCameraLookAtMatrixRH(const Vector3& position, const Vector3& lookAt, const Vector3& upVec)
{
	Vector3 zaxis = position - lookAt;
	zaxis.normalize();

	Vector3 xaxis = upVec.crossProduct(zaxis);
	xaxis.normalize();

	Vector3 yaxis = zaxis.crossProduct(xaxis);
	yaxis.normalize();
	
	M[0] = xaxis.X;
    M[1] = yaxis.X;
	M[2] = zaxis.X;
	M[3] = 0;
	
	M[4] = xaxis.Y;
	M[5] = yaxis.Y;
	M[6] = zaxis.Y;
	M[7] = 0;
	
	M[8] = xaxis.Z;
	M[9] = yaxis.Z;
	M[10] = zaxis.Z;
	M[11] = 0;

	M[12] = -xaxis.dotProduct(position);
	M[13] = -yaxis.dotProduct(position);
	M[14] = -zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}


inline Matrix4& Matrix4::BuildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	M[0] = 2*zNear/widthOfViewVolume;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;
	
	M[4] = 0;
	M[5] = 2*zNear/heightOfViewVolume;
	M[6] = 0;
	M[7] = 0;
	
	M[8] = 0;
	M[9] = 0;
	M[10] = zFar/(zNear-zFar);
	M[11] = -1;
	
	M[12] = 0;
	M[13] = 0;
	M[14] = zNear*zFar/(zNear-zFar);
	M[15] = 0;

	return *this;
}

inline Matrix4& Matrix4::BuildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const double h = 1.0 / tan(fieldOfViewRadians*0.5);
	const double w = h / aspectRatio;
	
	M[0] = (float)w; 
	M[1] = M[2] = M[3] = 0.0f;
	
	M[4] = M[6] = M[7] = 0.0f;
	M[5] = (float)h;

	M[8] = M[9] = 0.0f; M[11] = -1.0f;
	M[10] = (zFar+zNear/(zNear-zFar)); 

	M[12] = M[13] = M[15] = 0;
	M[14] = 2.0f*zNear*zFar/(zNear-zFar); 

	return *this; 
}


/*Vector4 Matrix4::GetColumnVector(int col) const 
{
	return Vector4(M[col], M[col+4], M[col+8], M[col+12]);
}*/

/*Vector4 Matrix4::GetRowVector(int row) const 
{
	return Vector4(M[row*4], M[row*4+1], M[row*4+2], M[row*4+3]);
}*/

#endif