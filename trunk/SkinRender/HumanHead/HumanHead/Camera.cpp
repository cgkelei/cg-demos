#include "Camera.h"
#include "Common.h"
#include <nvWidgets.h>

using namespace nv;

Camera::Camera(void)
	: mDistance(0.0f),
	mDistanceVelocity(0.0f),
	mAngle(0.0f, 0.0f),
	mAngularVelocity(0.0f, 0.0f),
	mPanPosition(0.0f, 0.0f),
	mPanVelocity(0.0f, 0.0f),
	mViewportSize(1.0f, 1.0f),
	mMousePos(0.0f, 0.0f),
	mAttenuation(0.0f),
	mDraggingLeft(false),
	mDraggingMiddle(false),
	mDraggingRight(false)
{
	UpdateMatrices();
}


Camera::~Camera(void)
{
}

void Camera::HandleMouse( int button, int state, int modifier, int x, int y )
{
	int modifierMask = 0;

	if ( button == GLUT_LEFT_BUTTON) button = MouseButton_Left;
	else if ( button == GLUT_MIDDLE_BUTTON) button = MouseButton_Middle;
	else if ( button == GLUT_RIGHT_BUTTON) button = MouseButton_Right;

	if ( modifier & GLUT_ACTIVE_ALT) modifierMask |= ButtonFlags_Alt;
	if ( modifier & GLUT_ACTIVE_SHIFT) modifierMask |= ButtonFlags_Shift;
	if ( modifier & GLUT_ACTIVE_CTRL) modifierMask |= ButtonFlags_Ctrl;

	if ( state == GLUT_DOWN) state = 1; else state = 0;

	mMousePos = glm::vec2(float(x), float(y));

	switch(button)
	{
	case MouseButton_Left: 
		{
			if (state)
			{
				mDraggingLeft = true;
				mAttenuation = 4.0f;
				//SetCapture(hwnd);
			}
			else
			{
				mDraggingLeft = false;
				mAttenuation = (modifier & ButtonFlags_Ctrl) ? 0.0f : 4.0f;
				//ReleaseCapture();
			}
		}
		break;

	case MouseButton_Right: 
		{
			if (state)
			{
				mDraggingRight = true;
				//SetCapture(hwnd);
			}
			else
			{
				mDraggingRight = false;
				//ReleaseCapture();
			}
		}
		break;

	case MouseButton_Middle:
		{
			if (state)
			{
				mDraggingMiddle = true;
				//SetCapture(hwnd);
			}
			else
			{
				mDraggingMiddle = false;
				//ReleaseCapture();
			}
		}
		break;
	default:
		break;
	}
}

void Camera::HandleMotion( int x, int y )
{
	//std::cout << x << " " << y << std::endl;

	glm::vec2 newMousePos = glm::vec2(float(x), float(y));

	if (mDraggingLeft)
	{
		glm::vec2 delta = newMousePos - mMousePos;
		mAngularVelocity -= delta;
		mMousePos = newMousePos;
	}

	if (mDraggingMiddle) 
	{
		glm::vec2 delta = newMousePos - mMousePos;
		UpdatePosition(delta);
		mMousePos = newMousePos;
	}

	if (mDraggingRight)
	{
		mDistance += (newMousePos.y - mMousePos.y) / 75.0f;
		mMousePos = newMousePos;
	}
}

void Camera::HandleMouseWheel( int value )
{
	mDistance -= float(value) / 400.0f;
}

void Camera::UpdatePosition(glm::vec2 delta)
{
	delta.x /= mViewportSize.x / 2.0f;
	delta.y /= mViewportSize.y / 2.0f;

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, mDistance));

	transform = mProjection * transform;

	glm::mat4 inverse =  transform._inverse();


	glm::vec4 t = glm::vec4(mPanPosition.x, mPanPosition.y, 0.0f, 1.0f);
	t = transform * t;
	
	t.x -= delta.x * t.w;
	t.y += delta.y * t.w;

	t = inverse * t;

	mPanPosition = glm::vec2(t.x, t.y);
}

void Camera::UpdateMatrices()
{
	mView = glm::translate(glm::mat4(1.0f), glm::vec3(-mPanPosition.x, -mPanPosition.y, -mDistance));

	glm::mat4 t;    
	t = glm::rotate(glm::mat4(1.0f),  mAngle.y, glm::vec3(1.0f, 0.0f, 0.0f));
	mView = mView * t ;

	t = glm::rotate(glm::mat4(1.0f),  -mAngle.x, glm::vec3(0.0f, 1.0f, 0.0f));
	mView = mView * t ;

	glm::mat4 viewInverse = glm::inverse(mView);

	glm::vec4 lookAtPosition4 = glm::vec4(0.0f, 0.0f, -mDistance, 1.0f);
	lookAtPosition4 = viewInverse * lookAtPosition4;
	mLookAt = glm::vec3(lookAtPosition4);

	//glm::vec4 eyePosition4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//eyePosition4 = viewInverse * eyePosition4  ;
	//mEyePosition =  glm::vec3(eyePosition4);
	
	mEyePosition = glm::vec3(glm::column(viewInverse, 3));

	mViewProj = mProjection * mView;


	static glm::mat4 bais(glm::vec4(0.5, 0, 0, 0), 
		glm::vec4(0, 0.5, 0, 0),
		glm::vec4(0, 0, 0.5, 0),
		glm::vec4(0.5, 0.5, 0.5, 1.0));

	mShadowMatrix =/* bais **/ mViewProj;
}

void Camera::Update( float elapsedTime )
{
	mAngle += mAngularVelocity * elapsedTime / 150.0f  * 180.0f / 3.1415926f;
	mAngularVelocity = mAngularVelocity / (1.0f + mAttenuation * elapsedTime);
	mDistance += mDistanceVelocity * elapsedTime / 150.0f;
	mPanPosition += mPanVelocity * elapsedTime / 150.0f;
	
	UpdateMatrices();
}

void Camera::SetProjection( float fov, float aspect, float nearPlane, float farPlane )
{
	mFov = fov; 
	mZFar = farPlane;
	mZNear = nearPlane;

	mProjection = glm::perspective(fov, aspect, nearPlane, farPlane);
}

//Camera& Camera::operator=( const Camera& cam )
//{
//	mDistance = cam.mDistance;
//	mDistanceVelocity = cam.mDistanceVelocity;
//	mPanPosition = cam.mPanPosition;
//	mPanVelocity = cam.mPanVelocity;
//	mAngle = cam.mAngle;
//	mAngularVelocity = cam.mAngularVelocity;
//	mViewportSize = cam.mViewportSize;
//	mView = cam.mView;
//	mProjection = cam.mProjection;
//	mLookAt = cam.mLookAt;
//	mEyePosition = cam.mEyePosition;
//	mAttenuation = cam.mAttenuation;
//	 
//	mDraggingLeft = false;
//	mDraggingMiddle = false;
//	mDraggingRight = false;
//
//
//	return *this;
//}

std::ostream &operator <<(std::ostream &os, const Camera &camera) 
{
	//const float scale = 180.0f / 3.1415926f;

	os << camera.mDistance << std::endl;
	os << camera.mAngle.x << " " << camera.mAngle.y << std::endl;
	os << camera.mPanPosition.x << " " << camera.mPanPosition.y << std::endl;
	os << camera.mAngularVelocity.x << " " << camera.mAngularVelocity.y << std::endl;
	return os;
}


std::istream &operator >>(std::istream &is, Camera &camera)
{
	const float scale = 180.0f / 3.1415926f;

	is >> camera.mDistance;  
	is >> camera.mAngle.x >> camera.mAngle.y;  /*camera.mAngle.x *= scale;  camera.mAngle.y *= scale;*/
	is >> camera.mPanPosition.x >> camera.mPanPosition.y;
	is >> camera.mAngularVelocity.x >> camera.mAngularVelocity.y;  //camera.mAngularVelocity.x *= scale;  camera.mAngularVelocity.y *= scale;
	return is;
}