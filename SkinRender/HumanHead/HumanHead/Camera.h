#pragma once

#include "Common.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	/*Camera& operator= (const Camera& cam);*/

	//void Reset() 
	//{
	//	mDraggingLeft = false;
	//	mDraggingMiddle = false;
	//	mDraggingRight = false;
	//}

	void HandleMouse(int button, int state, int modifier, int x, int y);
	void HandleMotion(int x, int y);
	void HandleMouseWheel(int value);

	void Update(float elapsedTime);

	void SetDistance(float distance) { mDistance = distance; }
	float GetDistance() const { return mDistance; }

	void SetDistanceVelocity(float distanceVelocity) { mDistanceVelocity = distanceVelocity; }
	float GetDistanceVelocity() const { return mDistanceVelocity; }

	void SetPanPosition(const glm::vec2 &panPosition) { mPanPosition = panPosition; }
	const glm::vec2& GetPanPosition() const { return mPanPosition; }

	void SetPanVelocity(const glm::vec2 &panVelocity) { mPanVelocity = panVelocity; }
	const glm::vec2& GetPanVelocity() const { return mPanVelocity; }

	void SetAngle(const glm::vec2 &angle) { mAngle = angle; }
	const glm::vec2& GetAngle() const { return mAngle; }

	void SetAngularVelocity(const glm::vec2 &angularVelocity) { mAngularVelocity = angularVelocity; }
	const glm::vec2& GetAngularVelocity() const { return mAngularVelocity; }

	void SetProjection(float fov, float aspect, float nearPlane, float farPlane);
	void SetViewportSize(const glm::vec2& viewportSize) { mViewportSize = viewportSize; }

	const glm::mat4& GetViewMatrix() { return mView; }
	const glm::mat4& GetProjectionMatrix() const { return mProjection; }
	const glm::mat4& GetViewProjectionMatrix() const { return mViewProj; }

	// multiply the bais matrix
	const glm::mat4& GetShadowMatrix() const { return mShadowMatrix; }

	const glm::vec3& GetLookAtPosition() { return mLookAt; }  
	const glm::vec3& GetEyePosition() { return mEyePosition; }  

	friend std::ostream& operator <<(std::ostream &os, const Camera &camera);
	friend std::istream& operator >>(std::istream &is, Camera &camera);



private:
	void UpdatePosition(glm::vec2 delta);
	void UpdateMatrices();

private:
	float mDistance, mDistanceVelocity;

	glm::vec2 mPanPosition, mPanVelocity;
	glm::vec2 mAngle, mAngularVelocity;

	glm::vec2 mViewportSize;

	glm::mat4 mView, mProjection, mViewProj, mShadowMatrix;

	glm::vec3 mLookAt, mEyePosition;

	glm::vec2 mMousePos;

	bool mDraggingLeft, mDraggingMiddle, mDraggingRight;
	float mAttenuation;
};

