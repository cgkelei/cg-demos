#ifndef Camera_h__
#define Camera_h__

#include <Core/Prerequisites.h>
#include <Math/MathUtil.h>

namespace RcEngine {

class _ApiExport Camera
{
public:
	Camera(void);
	~Camera(void);

	Vector3f GetPosition() const		{ return mPosition; }
	Vector3f GetLookAt() const			{ return mLookAt; }
	Vector3f GetUp() const				{ return mUpVec; }
	Vector3f GetView() const			{ return mViewVec; }
	float    GetFov() const				{ return mFieldOfView; }
	float    GetAspect() const			{ return mAspect; }
	float    GetNearPlane() const		{ return mNearPlane; }
	float	 GetFarPlane() const		{ return mFarPlane; }
	
	void SetViewParams(const Vector3f& eyePos, const Vector3f& lookat, const Vector3f& upVec = Vector3f(0, 1, 0));
	void SetProjectionParams(float fov, float aspect, float nearPlane, float farPlane);

	const Matrix4f& GetViewMatrix() const;
	const Matrix4f& GetProjectionMatrix() const;

private:
	Vector3f	mPosition;
	Vector3f	mLookAt;
	Vector3f	mUpVec;
	Vector3f	mViewVec;

	float		mFieldOfView;
	float		mAspect;
	float		mNearPlane;
	float		mFarPlane;

	Matrix4f	mViewMatrix;
	Matrix4f	mProjectionMatrix;
};

} // Namespace RcEngine

#endif // Camera_h__
