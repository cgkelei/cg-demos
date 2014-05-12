#ifndef DeferredPath_h__
#define DeferredPath_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderOperation.h>
#include <Math/ColorRGBA.h>
#include <Math/Matrix.h>
#include <Resource/Resource.h>

namespace RcEngine {

class CascadedShadowMap;
class RenderDevice;

class _ApiExport RenderPath
{
public:
	RenderPath();
	virtual ~RenderPath() {}

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height) {}
	virtual void RenderScene() {}

protected:
	void DrawFSQuad(const shared_ptr<Material>& material, const String& tech);
	void DrawOverlays();

protected:
	RenderDevice* mDevice;
	SceneManager* mSceneMan;
	shared_ptr<Camera> mCamera;
};

/**
 * Multiple pass forward shading.
 */
class _ApiExport ForwardPath : public RenderPath
{
public:
	ForwardPath();
	virtual ~ForwardPath() {}

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

	CascadedShadowMap* GetShadowManager() const  { return mShadowMan; }

public:
	CascadedShadowMap* mShadowMan;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;
};

/**
 * Stencil-Optimized light pre-pass deferred rendering.
 */
class _ApiExport DeferredPath : public RenderPath
{
public:
	DeferredPath();
	virtual ~DeferredPath() {}

	virtual void OnGraphicsInit(const shared_ptr<Camera>& camera);
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

	shared_ptr<Effect> GetDeferredEffect() const { return mDeferredEffect; } 
	CascadedShadowMap* GetShadowManager() const  { return mShadowMan; }

protected:
	void GenereateGBuffer();
	void ComputeSSAO();
	void DeferredLighting(); // Lighting pass
	void DeferredShading();  // Shading pass
	void PostProcess();


	void DrawDirectionalLightShape(Light* light);
	void DrawSpotLightShape(Light* light);
	void DrawPointLightShape(Light* light);

protected:

	CascadedShadowMap* mShadowMan;

	// Normal + Specular Shininess,  Albedo + Specular Intensity
	shared_ptr<Texture> mGBuffer[2];
	shared_ptr<RenderView> mGBufferRTV[2];
	
	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;
	shared_ptr<RenderView> mDepthStencilViewReadOnly;

	shared_ptr<Texture> mLightAccumulateBuffer;
	shared_ptr<Texture> mDepthStencilBufferLight; // Used for stencil optimization for light volume
	shared_ptr<RenderView> mLightAccumulateRTV;
	shared_ptr<RenderView> mDepthStencilBufferLightView;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<FrameBuffer> mGBufferFB;
	shared_ptr<FrameBuffer> mLightAccumulateFB;
	shared_ptr<FrameBuffer> mHDRFB;

	shared_ptr<Effect> mDeferredEffect;
	shared_ptr<Effect> mToneMapEffect;

	EffectTechnique* mDirLightTech;
	EffectTechnique* mPointLightTech;
	EffectTechnique* mSpotLightTech;
	EffectTechnique* mShadingTech;


	RenderOperation mSpotLightShape;
	RenderOperation mPointLightShape;
	RenderOperation mFullscreenTrangle;

	float4x4 mInvViewProj;
	float4x4 mViewProj;

public:
	bool mVisualLights;
	bool mVisualLightsWireframe;
};

class _ApiExport TileBasedDeferredPath
{

};

class _ApiExport TileBasedForwardPath
{

};

}


#endif // DeferredPath_h__
