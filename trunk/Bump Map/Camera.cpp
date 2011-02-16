#include "Camera.h"
#include "Application.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include <math.h>

Camera::Camera(void)
{
}

Camera::~Camera(void)
{
}


void Camera::initialMatrices()
{
	m_matView.MakeIdentity();
	
	Vector3 vAt(0.0f, 0.0f, 0.0f);
	Vector3 vUp(0.0f, 1.0f, 0.0f);
	m_matView.BuildCameraLookAtMatrixRH(m_position, vAt, vUp);

	float width = (float)Application::Instance()->GetWidth();
	float height = (float)Application::Instance()->GetHeight();
	m_matProj.BuildProjectionMatrixPerspectiveFovRH(M_PI/4, width / height, 0.1f, 100.0f);
	
}


void Camera::updateMatrices()
{
	m_matView.MakeIdentity();

	Matrix4  matTranslation;
	matTranslation.BuildTranslateMatrix(-m_position.X, -m_position.Y, -m_position.Z);
	m_matView *= matTranslation;

	Matrix4 matRotation;
	matRotation.BuildRotationYMatrix(m_rotation.Y);
	m_matView *= matRotation;

	matRotation.BuildRotationXMatrix(m_rotation.X);
	m_matView *= matRotation;

	float width = (float)Application::Instance()->GetWidth();
	float height = (float)Application::Instance()->GetHeight();

	m_matProj.BuildProjectionMatrixPerspectiveFovRH(M_PI/4, width / height, 0.1f, 100.0f);
}

//旋转摄像机
void Camera::updateRotation()
{

}

//移动摄像机
void Camera::updateTranslate()
{
	Vector3 vecMovement;
	const KeyboardHandler* pKeyboardHandler = Application::Instance()->GetKeyboardHandler();
	
	if (pKeyboardHandler->isKeyPressed('W'))
	{
		vecMovement.X += -(float)sin(m_rotation.Y);
		vecMovement.Z +=  (float)cos(m_rotation.Y);
	}
	else if (pKeyboardHandler->isKeyPressed('S'))
	{
		vecMovement.X +=  (float)sin(m_rotation.Y);
		vecMovement.Z += -(float)cos(m_rotation.Y);
	}

	if (pKeyboardHandler->isKeyPressed('A'))
	{
		vecMovement.X += -(float)cos(m_rotation.Y);
		vecMovement.Z += -(float)sin(m_rotation.Y);
	}
	else if (pKeyboardHandler->isKeyPressed('D'))
	{
		vecMovement.X += (float)cos(m_rotation.Y);
		vecMovement.Z += (float)sin(m_rotation.Y);
	}

	if (pKeyboardHandler->isKeyPressed('Q'))
	{
		vecMovement.Y += 0.8f;
	}
	else if (pKeyboardHandler->isKeyPressed('E'))
	{
		vecMovement.Y -= 0.8f;
	}

	vecMovement.normalize();
	vecMovement *= 8;

}

void Camera::positionCamera(float positionX, float positionY, float positionZ,
							float viewX, float viewY, float viewZ, 
							float upVectorX, float upVectorY, float upVectorZ)
{
	m_position = Vector3(positionX, positionY, positionZ);
	m_upVec = Vector3(upVectorX, upVectorY, upVectorZ);
	m_lookAt = Vector3(viewX, viewY, viewZ);
	m_viewVec = m_lookAt - m_position;

	updateMatrices();
}

void Camera::rotateView( float angle, float x, float y, float z )
{

	Vector3 newViewVec;
	Vector3 viewVec(m_lookAt - m_position);
	
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);
	
	Vector3 newLookAt;
	
	newViewVec.X = (cosTheta + (1 - cosTheta) * x * x) * viewVec.X;
	newViewVec.X += ((1 - cosTheta) * x * y - z * sinTheta) * viewVec.Y;
	newViewVec.X += (( 1- cosTheta) * x * z + y * sinTheta) * viewVec.Z;

	newViewVec.Y = ((1 - cosTheta) * x * y + z * sinTheta) * viewVec.X;
	newViewVec.Y += (cosTheta + (1 - cosTheta) * y * y) * viewVec.Y;
	newViewVec.Y += ((1 - cosTheta) * y * z - x * sinTheta) * viewVec.Z;

	newViewVec.Z = ((1 - cosTheta) * x * z - y * sinTheta) * viewVec.X;
	newViewVec.Z += ((1 - cosTheta) * y * z + x * sinTheta) * viewVec.Y;
	newViewVec.Z += (cosTheta + (1 - cosTheta) * z * z) * viewVec.Z;

	m_lookAt =  newViewVec + m_position;

}
