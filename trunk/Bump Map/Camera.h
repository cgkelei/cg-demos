#ifndef _Camera__H
#define _Camera__H

#include "Matrix.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

public:
	const Matrix4& getWorldMatrix() const 		{ return m_matWorld; }
	const Matrix4& getViewMatrix()	const 		{ return m_matView;}
	const Matrix4& getProjectionMatrix() const 	{ return m_matProj;}
	
	void positionCamera(float positionX, float positionY, float positionZ,
						float viewX, float viewY, float viewZ, 
						float upVectorX, float upVectorY, float upVectorZ) ;
	
	void rotateView(float angle, float x, float y, float z);

protected:
	void initialMatrices();
	void updateMatrices();
	void updateRotation();
	void updateTranslate();

private:
	Matrix4		m_matWorld;
	Matrix4		m_matView;
	Matrix4		m_matProj;

	Vector3		m_position;
	Vector3     m_lookAt;
	Vector3		m_upVec;
	Vector3		m_viewVec;

	Vector3		m_rotation;


};


#endif