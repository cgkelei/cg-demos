#include <Graphics/Camera.h>
#include <Core/Context.h>
#include <Graphics/RenderDevice.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Camera::Camera(void)
	: mFrustumDirty(true)
{
	SetViewParams(float3(0, 0, 0), float3(0, 0, 1), float3(0, 1, 0));
	SetProjectionParams(Mathf::PI / 4.0f, 1.0f, 1.0f, 1000.0f);
}


Camera::~Camera(void)
{
}

void Camera::SetViewParams( const float3& eyePos, const float3& lookat, const float3& upVec /*= float3(0, 1, 0)*/ )
{
	mPosition = eyePos;
	mLookAt	= lookat;
	mUpVec	= upVec;

	mViewVec = Normalize(mLookAt - mPosition);
	mViewMatrix = CreateLookAtMatrixLH(mPosition, mLookAt, mUpVec);
	mInvViewMatrix = mViewMatrix.Inverse();

	mFrustumDirty = true;
}

void Camera::SetProjectionParams( float fov, float aspect, float nearPlane, float farPlane )
{
	mFieldOfView = fov;
	mAspect = aspect;
	mNearPlane = nearPlane;
	mFarPlane = farPlane;

	mProjMatrix = CreatePerspectiveFovLH(mFieldOfView, mAspect, mNearPlane, mFarPlane);
	mInvProjMatrix = mProjMatrix.Inverse();

	//修改投影矩阵，使OpenGL适应左右坐标系
	Context::GetSingletonPtr()->GetRenderDevicePtr()->AdjustProjectionMatrix(mProjMatrix);

	mFrustumDirty = true;
}

const Frustumf& Camera::GetFrustum() const
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}
	return mFrustum;
}

bool Camera::Visible( const BoundingSpheref& sphere )
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}

	if (mFrustum.Contain(sphere) == CT_Disjoint)
		return false;

	return true;
}

bool Camera::Visible( const BoundingBoxf& box )
{
	if (mFrustumDirty)
	{
		mFrustum.Update(mViewMatrix * mProjMatrix);
		mFrustumDirty = false;
	}

	if (mFrustum.Contain(box) == CT_Disjoint)
		return false;

	return true;
}

} // Namespace RcEngine