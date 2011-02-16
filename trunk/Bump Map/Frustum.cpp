#include "Frustum.h"
#include <gl/GL.h>

void Frustum::CalculateFrustum(const Matrix4& wvpMatrix)
{
	m_cullingMatrix = wvpMatrix;
	
	// Left clipping plane
	m_plane[FRUSTUM_PLANE_LEFT].SetPlane(m_cullingMatrix.GetColumnVector(4) + 
											m_cullingMatrix.GetColumnVector(1));
	m_plane[FRUSTUM_PLANE_LEFT].Normalize();

	//right clipping plane
	m_plane[FRUSTUM_PLANE_FAR].SetPlane(m_cullingMatrix.GetColumnVector(4) -
											m_cullingMatrix.GetColumnVector(1));
	m_plane[FRUSTUM_PLANE_FAR].Normalize();
	
	// Top clipping plane
	m_plane[FRUSTUM_PLANE_TOP].SetPlane(m_cullingMatrix.GetColumnVector(4) -
											m_cullingMatrix.GetColumnVector(2));
	m_plane[FRUSTUM_PLANE_TOP].Normalize();

	// Bottom clipping plane
	m_plane[FRUSTUM_PLANE_BOTTOM].SetPlane(m_cullingMatrix.GetColumnVector(4) +
											m_cullingMatrix.GetColumnVector(2));
	m_plane[FRUSTUM_PLANE_BOTTOM].Normalize();

	// Near clipping plane
	m_plane[FRUSTUM_PLANE_NEAR].SetPlane(m_cullingMatrix.GetColumnVector(4) +
											m_cullingMatrix.GetColumnVector(3));
	m_plane[FRUSTUM_PLANE_NEAR].Normalize();
		
	// Far clipping plane
	m_plane[FRUSTUM_PLANE_FAR].SetPlane(m_cullingMatrix.GetColumnVector(4) -
											m_cullingMatrix.GetColumnVector(3));
	m_plane[FRUSTUM_PLANE_FAR].Normalize();
}

bool Frustum::PointInFrustum( float x, float y, float z )
{
	for(int i = 0; i < 6; i++ )
	{
		if(m_plane[i].ClassifyPoint(x, y, z) < 0)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::SphereInFrustum( float x, float y, float z, float radius )
{
	for(int i = 0; i < 6; i++ )	
	{
		if(  m_plane[i].ClassifyPoint(x, y, z) <= -radius )
		{
			return false;
		}
	}
	return true;
}

bool Frustum::CubeInFrustum( float x, float y, float z, float size )
{
	return true;
}

Frustum::Frustum( float fov, float ratio, float near, float far )
	: m_fFov(fov), m_fFar(far), m_fNear(near), m_fRatio(ratio)
{
}

void Frustum::UpdateFrustum( Vector3 &viewpos, Vector3 &viewdir, Vector3 &viewupvec )
{

	Vector3 vecRight = viewdir.crossProduct(viewupvec); 
	vecRight.normalize(); 

	Vector3 vecUp = vecRight.crossProduct(viewdir); 
	vecUp.normalize(); 

	GLfloat radianFov = GLfloat(m_fFov * M_PI/180.0f) ; 

	GLfloat nearHeightHalf = m_fNear * (GLfloat)tan(radianFov/2.0f); 
	GLfloat nearWidthHalf  = nearHeightHalf * m_fRatio; 
	GLfloat farHeightHalf  = m_fFar  * (GLfloat)tan(radianFov/2.0f); 
	GLfloat farWidthHalf   = farHeightHalf *  m_fRatio; 


	Vector3 nearCenter = viewpos + viewdir * m_fNear; 
	Vector3 farCenter = viewpos + viewdir * m_fNear; 

	m_points[0] = nearCenter - vecUp*nearHeightHalf - vecRight*nearWidthHalf; 
	m_points[1] = nearCenter - vecUp*nearHeightHalf + vecRight*nearWidthHalf;
	m_points[2] = nearCenter + vecUp*nearHeightHalf + vecRight*nearWidthHalf;
	m_points[3] = nearCenter + vecUp*nearHeightHalf - vecRight*nearWidthHalf;

	m_points[4] = nearCenter - vecUp*farHeightHalf - vecRight*farWidthHalf; 
	m_points[5] = nearCenter - vecUp*farHeightHalf + vecRight*farWidthHalf;
	m_points[6] = nearCenter + vecUp*farHeightHalf + vecRight*farWidthHalf;
	m_points[7] = nearCenter + vecUp*farHeightHalf - vecRight*farWidthHalf;

}

void Frustum::DrawFrustum( bool linemode, ColorRGBA col, Vector3 &viewpos, bool nearPlane /*= false*/, bool farPlane /*= false*/ )
{

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	glDisable(GL_LIGHTING); 
	if(linemode)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	glPushMatrix(); 
	glColor4f(col.R(), col.G(), col.B(), col.A()); 
	glLineWidth(1.0); 
	glPointSize(10); 

	if(nearPlane) 
	{ 
		glBegin(GL_QUADS); 
		for(int j=0; j<4; j++) 
			glVertex3d(m_points[j].X, m_points[j].Y, m_points[j].Z); 
		glEnd(); 
	} 
	if(farPlane) 
	{ 
		glBegin(GL_QUADS); 
		for(int i=4; i<8; i++) 
			glVertex3d(m_points[i].X, m_points[i].Y, m_points[i].Z); 
		glEnd(); 
	} 

	for(int k=0; k<3; k++) 
	{ 
		glBegin(GL_QUADS); 
		glVertex3d(m_points[k].X, m_points[k].Y, m_points[k].Z); 
		glVertex3d(m_points[k+4].X, m_points[k+4].Y, m_points[k+4].Z); 
		glVertex3d(m_points[k+5].X, m_points[k+5].Y, m_points[k+5].Z); 
		glVertex3d(m_points[k+1].X, m_points[k+1].Y, m_points[k+1].Z); 
		glEnd(); 
	} 

	glBegin(GL_QUADS); 
	glVertex3d(m_points[3].X, m_points[3].Y, m_points[3].Z); 
	glVertex3d(m_points[7].X, m_points[7].Y, m_points[7].Z); 
	glVertex3d(m_points[4].X, m_points[4].Y, m_points[4].Z); 
	glVertex3d(m_points[0].X, m_points[0].Y, m_points[0].Z); 
	glEnd(); 

	for(int l=0; l<4; l++) 
	{ 
		glBegin(GL_LINES); 
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f); 
		glVertex3d(viewpos.X, viewpos.Y, viewpos.Z); 
		glVertex3d(m_points[l+4].X, m_points[l+4].Y, m_points[l+4].Z); 
		glEnd(); 
	} 
	glPopMatrix(); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
	glDisable(GL_BLEND); 
}
