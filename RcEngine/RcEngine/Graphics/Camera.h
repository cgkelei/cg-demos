#ifndef Camera_h__
#define Camera_h__

#include <Core/Prerequisites.h>
#include <Math/Frustum.h>

namespace RcEngine {

class _ApiExport Camera
{
public:
	Camera(void);
	~Camera(void);

	const float3& GetPosition() const			{ return mPosition; }
	const float3& GetLookAt() const				{ return mLookAt; }
	const float3& GetUp() const					{ return mUpVec; }
	const float3& GetView() const				{ return mViewVec; }
	float  GetFov() const						{ return mFieldOfView; }
	float  GetAspect() const					{ return mAspect; }
	float  GetNearPlane() const					{ return mNearPlane; }
	float  GetFarPlane() const					{ return mFarPlane; }
	
	void SetViewParams(const float3& eyePos, const float3& lookat, const float3& upVec = float3(0, 1, 0));
	void SetProjectionParams(float fov, float aspect, float nearPlane, float farPlane);

	const float4x4& GetViewMatrix() const				{ return mViewMatrix; }
	const float4x4& GetInvViewMatrix() const			{ return mInvViewMatrix; }

	const float4x4& GetProjectionMatrix() const			{ return mProjMatrix; }
	const float4x4& GetInvProjectionMatrix() const		{ return mInvProjMatrix;}

	const Frustumf& GetFrustum() const;

	bool Visible( const BoundingSpheref& sphere );
	bool Visible( const BoundingBoxf& box );

private:
	float3 mPosition;
	float3 mLookAt;
	float3 mUpVec;
	float3 mViewVec;

	float mFieldOfView;
	float mAspect;
	float mNearPlane;
	float mFarPlane;

	float4x4 mViewMatrix;
	float4x4 mProjMatrix;

	float4x4 mInvProjMatrix;
	float4x4 mInvViewMatrix;

	mutable Frustumf mFrustum;
	mutable bool mFrustumDirty;
};

} // Namespace RcEngine

#endif // Camera_h__
