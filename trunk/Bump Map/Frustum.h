#ifndef _Frustum__H
#define _Frustum__H

#include "Plane.h"
#include "Matrix.h"
#include "ColorRGBA.h"

enum FrustumPlane
{
	FRUSTUM_PLANE_FAR = 0, 
	FRUSTUM_PLANE_NEAR, 
	FRUSTUM_PLANE_LEFT, 
	FRUSTUM_PLANE_RIGHT, 
	FRUSTUM_PLANE_TOP,
	FRUSTUM_PLANE_BOTTOM
};

class Frustum
{
public:
	Frustum() {} 
	Frustum(float fov, float ratio, float near, float far);
	Frustum(const Matrix4& vpMatrix);
	
	//Calculate 6 Frustum Plane based on view and projextion matrix
	void CalculateFrustum(const Matrix4& vpMatrix);
	
	//Draw six plane
	void UpdateFrustum(Vector3 &viewpos, Vector3 &viewdir, Vector3 &viewupvec); 
	void DrawFrustum(bool linemode, ColorRGBA col, Vector3 &viewpos, bool nearPlane = false, bool farPlane = false); 




	bool PointInFrustum( float x, float y, float z );
	bool SphereInFrustum( float x, float y, float z, float radius );
	bool CubeInFrustum( float x, float y, float z, float size );
	
private:
	Plane		m_plane[6];
	Matrix4		m_cullingMatrix;

	//Draw Frustum

	float		m_fNear; 
	float		m_fFar; 
	float		m_fFov; 
	float		m_fRatio; 
	Vector3		m_points[8]; 
};





#endif