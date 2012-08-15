#include <Graphics/Camera.h>
#include <Core/Context.h>
#include <Graphics/RenderDevice.h>

namespace RcEngine {
namespace Render {

Camera::Camera(void)
{
	SetViewParams(Vector3f(0, 0, 0), Vector3f(0, 0, 1), Vector3f(0, 1, 0));
	SetProjectionParams(PI / 4.0f, 1.0f, 1.0f, 1000.0f);
}


Camera::~Camera(void)
{
}

void Camera::SetViewParams( const Vector3f& eyePos, const Vector3f& lookat, const Vector3f& upVec /*= Vector3f(0, 1, 0)*/ )
{
	mPosition = eyePos;
	mLookAt	= lookat;
	mUpVec	= upVec;

	mViewVec = Normalize(mLookAt - mPosition);
	mViewMatrix = CreateLookAtMatrixLH(mPosition, mLookAt, mUpVec);
}

void Camera::SetProjectionParams( float fov, float aspect, float nearPlane, float farPlane )
{
	mFieldOfView = fov;
	mAspect = aspect;
	mNearPlane = nearPlane;
	mFarPlane = farPlane;

	mProjectionMatrix = CreatePerspectiveFovLH(mFieldOfView, mAspect, mNearPlane, mFarPlane);
	//修改投影矩阵，使OpenGL适应左右坐标系
	Core::Context::GetSingletonPtr()->GetRenderDevicePtr()->AdjustProjectionMatrix(mProjectionMatrix);
}

const Matrix4f& Camera::GetViewMatrix() const
{
	return mViewMatrix;
}

const Matrix4f& Camera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

} // Namespace Render
} // Namespace RcEngine