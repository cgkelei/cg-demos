#include <Graphics/CascadedShadowMap.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Light.h>
#include <Core/Context.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Texture.h>
#include <Graphics/Camera.h>
#include <Math/MathUtil.h>

namespace {

using namespace RcEngine;

void FrustumPlaneExtraction(const Camera& camera, float camZ, float3 corner[4])
{
	float height = camZ * tanf(camera.GetFov() * 0.5f);
	float width = height * camera.GetAspect();

	float3 center = camera.GetPosition() + camera.GetView() * camZ;
	float3 right = camera.GetRight();
	float3 up = camera.GetUp();

	corner[0] = center - width * right - height * up;
	corner[1] = center - width * right + height * up;
	corner[2] = center + width * right + height * up;
	corner[3] = center + width * right - height * up;
}


}

namespace RcEngine {

CascadedShadowMap::CascadedShadowMap()
{
	mLightViewProj.resize(MAX_CASCADES);
	for (int i = 0; i < MAX_CASCADES; ++i)
		mLightCamera.push_back(std::make_shared<Camera>());
}

void CascadedShadowMap::UpdateShadowMapSize( const Light& light )
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	uint32_t numCascade = light.GetShadowCascades();
	uint32_t shadowTexWidth = SHADOW_MAP_SIZE * numCascade;
	uint32_t shadowTexHeight = SHADOW_MAP_SIZE;

	if (!mShadowTexture)
	{
	mShadowFrameBuffer = factory.CreateFrameBuffer(shadowTexWidth, shadowTexHeight);
	mShadowTexture = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_Depth32, 1, 0, 1, 0, 0, NULL);

	device.BindFrameBuffer(mShadowFrameBuffer);
	mShadowFrameBuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(mShadowTexture, 0, 0));
	mShadowFrameBuffer->Clear(CF_Depth, ColorRGBA::Black, 1.0f, 0);
	}
	else if (mShadowTexture->GetWidth(0) != shadowTexWidth || mShadowTexture->GetHeight(0) != shadowTexHeight)
	{
	mShadowFrameBuffer->Resize(shadowTexWidth, shadowTexHeight);
	mShadowTexture = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_Depth32, 1, 0, 1, 0, 0, NULL);

	device.BindFrameBuffer(mShadowFrameBuffer);
	mShadowFrameBuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(mShadowTexture, 0, 0));
	mShadowFrameBuffer->Clear(CF_Depth, ColorRGBA::Black, 1.0f, 0);
	}
	else
	{
	mShadowFrameBuffer->SetViewport(Viewport(0, 0, shadowTexWidth, shadowTexHeight));
	device.BindFrameBuffer(mShadowFrameBuffer);	
	mShadowFrameBuffer->Clear(CF_Depth, ColorRGBA::Black, 1.0f, 0);
	}

	/*if (!mShadowTexture)
	{
		mShadowFrameBuffer = factory.CreateFrameBuffer(shadowTexWidth, shadowTexHeight);
		mShadowDepth = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_Depth32, 1, 0, 1, 0, EAH_GPU_Write, NULL);
		mShadowTexture = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_R32F, 1, 0, 1, 0, EAH_GPU_Write | EAH_GPU_Read, NULL);

		device.BindFrameBuffer(mShadowFrameBuffer);
		mShadowFrameBuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(mShadowDepth, 0, 0));
		mShadowFrameBuffer->Attach(ATT_Color0, factory.CreateRenderTargetView2D(mShadowTexture, 0, 0));
		mShadowFrameBuffer->Clear(CF_Depth | CF_Color, ColorRGBA(1, 1, 1, 1), 1.0f, 0);
	}
	else if (mShadowTexture->GetWidth(0) != shadowTexWidth || mShadowTexture->GetHeight(0) != shadowTexHeight)
	{
		mShadowFrameBuffer->Resize(shadowTexWidth, shadowTexHeight);
		mShadowDepth = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_Depth32, 1, 0, 1, 0, EAH_GPU_Write, NULL);
		mShadowTexture = factory.CreateTexture2D(shadowTexWidth, shadowTexHeight, PF_R32F, 1, 0, 1, 0, EAH_GPU_Write | EAH_GPU_Read, NULL);

		device.BindFrameBuffer(mShadowFrameBuffer);
		mShadowFrameBuffer->Attach(ATT_DepthStencil, factory.CreateDepthStencilView(mShadowDepth, 0, 0));
		mShadowFrameBuffer->Attach(ATT_Color0, factory.CreateRenderTargetView2D(mShadowTexture, 0, 0));
		mShadowFrameBuffer->Clear(CF_Depth | CF_Color, ColorRGBA(1, 1, 1, 1), 1.0f, 0);
	}
	else
	{
		mShadowFrameBuffer->SetViewport(Viewport(0, 0, shadowTexWidth, shadowTexHeight));
		device.BindFrameBuffer(mShadowFrameBuffer);	
		mShadowFrameBuffer->Clear(CF_Depth | CF_Color, ColorRGBA(1, 1, 1, 1), 1.0f, 0);
	}*/

	for (uint32_t i = 0; i < numCascade; ++i)
		mShadowVP[i] = Viewport(SHADOW_MAP_SIZE*i, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
}

void CascadedShadowMap::UpdateShadowMatrix( const Camera& camera, const Light& light )
{
	// Update shadow map texture
	UpdateShadowMapSize(light);

	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();

	const Frustumf& camFrustum = camera.GetFrustum();
	const float3& camPos = camera.GetPosition();
	const float3& camDir = camera.GetView();
	const float3& camUp = camera.GetUp();

	const uint32_t numSplits= light.GetShadowCascades();
	const float splitLambda = light.GetSplitLambda();
	const float3& lightDirection = light.GetDerivedDirection();

	float3 lightUp = camera.GetRight();
	if(fabs(Dot(lightUp,lightDirection))>0.9f) lightUp = camera.GetUp();

	for (uint32_t i = 0; i < numSplits; ++i)
		mLightCamera[i]->CreateLookAt(float3(0, 0, 0), lightDirection, lightUp);

	// Keep a copy
	float4x4 lightView = mLightCamera[0]->GetViewMatrix();

	// 0. Find scene AABB. exclude those never cast shadow like terrain.
	BoundingBoxf sceneAABB = sceneMan.GetRootSceneNode()->GetWorldBoundingBox();
	sceneAABB = Transform(sceneAABB, lightView);

	// 1. Split view frustum into sub frustum
	float nearPlane = camera.GetNearPlane();
	float farPlane = camera.GetFarPlane();

	uint32_t iSplit = 0;
	mSplitPlanes[iSplit] = nearPlane;
	for (iSplit = 1; iSplit < numSplits; ++iSplit)
	{
		float f = float(iSplit) / numSplits;
		float logSplit = nearPlane * powf(farPlane/nearPlane, f);
		float uniSplit = nearPlane + (farPlane - nearPlane) * f;
		mSplitPlanes[iSplit] = splitLambda * logSplit + (1.0f - splitLambda) * uniSplit;
	}
	mSplitPlanes[iSplit] = farPlane;

	// 2. Compute an orthographic projection for each sub frustum.
	int32_t nearSplitIdx = 0, farSplitIdx = 1;
	float3 Corners[2][4];
	FrustumPlaneExtraction(camera, mSplitPlanes[0], Corners[nearSplitIdx]);

	// Transform world space to light view space
	for (int i = 0; i < 4; ++i) 
		Corners[nearSplitIdx][i] = Transform(Corners[nearSplitIdx][i], lightView);

	for (iSplit = 0; iSplit < numSplits; ++iSplit)
	{
		// Compute AABB for sub frustum.
		FrustumPlaneExtraction(camera, mSplitPlanes[iSplit+1], Corners[farSplitIdx]);
		for (int i = 0; i < 4; ++i)
			Corners[farSplitIdx][i] = Transform(Corners[farSplitIdx][i], lightView);

		BoundingBoxf boundSplit;
		for (int i = 0; i < 4; ++i)
		{
			boundSplit.Merge(Corners[nearSplitIdx][i]);
			boundSplit.Merge(Corners[farSplitIdx][i]);
		}

		boundSplit.Min.X() = (std::max)(boundSplit.Min.X(), sceneAABB.Min.X());
		boundSplit.Min.Y() = (std::max)(boundSplit.Min.Y(), sceneAABB.Min.Y());
		boundSplit.Max.X() = (std::min)(boundSplit.Max.X(), sceneAABB.Max.X());
		boundSplit.Max.Y() = (std::min)(boundSplit.Max.Y(), sceneAABB.Max.Y());
		boundSplit.Min.Z() = sceneAABB.Min.Z();
		boundSplit.Max.Z() = (std::min)(boundSplit.Max.Z(), sceneAABB.Max.Z());

		// Ping-Pang swap
		std::swap(nearSplitIdx, farSplitIdx);

		// Build ortho projection matrix
		mLightCamera[iSplit]->CreateOrthoOffCenter(boundSplit.Min.X(), boundSplit.Max.X(),
												   boundSplit.Min.Y(), boundSplit.Max.Y(),
												   boundSplit.Min.Z(), boundSplit.Max.Z());

		mLightViewProj[iSplit] = lightView * mLightCamera[iSplit]->GetProjMatrix();
	}
}

}